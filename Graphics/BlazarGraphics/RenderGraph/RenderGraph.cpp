/*
Blazar Engine - 3D Game Engine
Copyright (c) 2020-2021 Muhammed Murat Cengiz

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "RenderGraph.h"

#include <utility>
#include <boost/format.hpp>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

RenderGraph::RenderGraph( IRenderDevice* renderDevice, AssetManager* assetManager ) : renderDevice( renderDevice ), assetManager( assetManager )
{
	globalResourceTable = std::make_unique< GlobalResourceTable >( this->renderDevice, this->assetManager );
}

void RenderGraph::addPass( Pass * pass )
{
	FUNCTION_BREAK( passMap.find( pass->name ) != passMap.end( ) )

	PassWrapper& wrapper = passes.emplace_back( );
	wrapper.renderPass = nullptr;
	wrapper.ref = pass;

	passMap[ wrapper.ref->name ] = passes.size( ) - 1;
}

void RenderGraph::clearGraph( )
{
    for ( auto& pass : passes )
    {
        pass.pipelines.clear( );
        pass.pipelineInputsMap.clear( );
        pass.pipelineInputsFlat.clear( );
        pass.dependencies.clear( );
    }

    this->pipelineInputOutputDependencies.clear( );
    this->frameLocks.clear( );
    passes.clear( );
}

void RenderGraph::buildGraph( )
{
    // flatten pipeline input, saves a loop later
	for ( auto& pass : passes )
	{
		uint32_t pipelineIndex = 0;

		pass.pipelineInputsMap.resize( pass.ref->pipelineInputs.size( ) );

		for ( const auto& pipelineInputs : pass.ref->pipelineInputs )
		{
			for ( const auto& input : pipelineInputs )
			{
				if ( pass.pipelineInputsMap[ pipelineIndex ].find( input ) == pass.pipelineInputsMap[ pipelineIndex ].end( ) )
				{
					pass.pipelineInputsFlat.push_back( input );
					pass.pipelineInputsMap[ pipelineIndex ][ input ] = true;
				}
			}

			pipelineIndex++;
		}
	}

	// build dependencies
	for ( auto& firstPass : passes )
	{
		for ( const auto& secondPass : passes )
		{
			for ( const auto& firstPassInput : firstPass.pipelineInputsFlat )
			{
				for ( const auto& secondPassOutput : secondPass.ref->outputs )
				{
					if ( firstPassInput == secondPassOutput.outputResourceName )
					{
						firstPass.dependencies.push_back( secondPass.ref->name );
					}
				}
			}
		}
	}

	// Prepare input-output map
	for ( const auto& pass : passes )
	{
		for ( const auto& dependency : pass.dependencies )
		{
			PassWrapper& dependencyPass = passes[ passMap[ dependency ] ];
			for ( const auto& input : pass.pipelineInputsFlat )
			{
				for ( const auto& dependencyOutput : dependencyPass.ref->outputs )
				{
					if ( input == dependencyOutput.outputResourceName )
					{
						this->pipelineInputOutputDependencies[ input ] = dependency;
					}
				}
			}
		}
	}

	for ( int i = 0; i < renderDevice->getFrameCount( ); ++i )
	{
		frameLocks.push_back( std::make_unique< std::mutex >( ) );

		// todo support threading
	}
}

void RenderGraph::prepare( ECS::ComponentTable * componentTable )
{
	// Last pass should always be the render pass!
	PassWrapper& lastPass = passes[ passes.size( ) - 1 ];
	if ( !lastPass.executeLocks.empty( ) ) // make sure it is not the very first frame
	{
		lastPass.executeLocks[ frameIndex ]->wait( );
	}

	globalResourceTable->resetTable( componentTable, frameIndex );

	for ( auto& pass : passes )
	{
		preparePass( pass );
	}
}

void RenderGraph::preparePass( PassWrapper& pass )
{
	// Initialize inputs
	if ( !pass.inputsBuilt )
	{
		prepareInputs( pass );
		pass.inputsBuilt = true;
	}

	if ( pass.renderPass == nullptr )
	{
		pass.ref->renderPassRequest.outputImages = pass.ref->outputs;
		pass.renderPass = renderDevice->getRenderPassProvider( )->createRenderPass( pass.ref->renderPassRequest );
		pass.renderPass->create( pass.ref->renderPassRequest );
	}

	if ( pass.renderTargets.empty( ) )
	{
		RenderTargetRequest renderTargetRequest{ };
		renderTargetRequest.renderPass = pass.renderPass;

		pass.renderTargets.resize( renderDevice->getFrameCount( ) );

		for ( uint32_t frame = 0; frame < pass.renderTargets.size( ); ++frame )
		{
			renderTargetRequest.frameIndex = frame;
			renderTargetRequest.type = RenderTargetType::Intermediate;
			renderTargetRequest.renderArea = pass.ref->renderPassRequest.renderArea;

			for ( const auto& output : pass.ref->outputs )
			{
				if ( output.flags.presentedImage )
				{
					renderTargetRequest.type = RenderTargetType::SwapChain;
				}
			}

			renderTargetRequest.outputImages = pass.ref->outputs;

			pass.renderTargets[ frame ] = renderDevice->getRenderPassProvider( )->createRenderTarget( renderTargetRequest );
		}
	}

	if ( pass.pipelines.empty( ) )
	{
		int pipelineIdx = 0;
		pass.pipelines.resize( pass.ref->pipelineRequests.size( ) );

		for ( auto& pipelineRequest : pass.ref->pipelineRequests )
		{
			pipelineRequest.parentPass = pass.renderPass;
			pass.pipelines[ pipelineIdx ] = renderDevice->getPipelineProvider( )->createPipeline( pipelineRequest );
			++pipelineIdx;
		}
	}

	if ( pass.executeLocks.empty( ) )
	{
        for ( int i = 0; i < renderDevice->getFrameCount(); ++i ) {
            pass.executeLocks.push_back( std::move( renderDevice->getResourceProvider( )->createLock( ResourceLockType::Fence ) ) );
        }
	}
}

void RenderGraph::prepareInputs( PassWrapper& pass ) const
{
	pass.passDependentInputs.resize( pass.ref->pipelineInputs.size( ) );
	pass.perGeometryInputs.resize( pass.ref->pipelineInputs.size( ) );
	pass.perEntityInputs.resize( pass.ref->pipelineInputs.size( ) );
	pass.loadOnceInputs.resize( pass.ref->pipelineInputs.size( ) );
	pass.perFrameInputs.resize( pass.ref->pipelineInputs.size( ) );

	int pipelineIdx = 0;

	for ( const auto& pipelineInput : pass.ref->pipelineInputs )
	{
		for ( auto& input : pipelineInput )
		{
			ShaderUniformBinder* pBinder = globalResourceTable->getResourceBinder( );

			if ( !pBinder->checkBinderExistsByName( input ) )
			{
				// Assume input is generated in another pass
				pass.passDependentInputs[ pipelineIdx ].push_back( input );
				continue;
			}

			const auto binder = pBinder->getBinderByName( input );
			int binderIdx = pBinder->getBinderIdx( input );

			switch ( binder.frequency )
			{
			case UpdateFrequency::Once:
				pass.loadOnceInputs[ pipelineIdx ].push_back( binderIdx );
				break;
			case UpdateFrequency::EachGeometry:
				pass.perGeometryInputs[ pipelineIdx ].push_back( binderIdx );
				globalResourceTable->addPerGeometryResource( binderIdx );
				break;
			case UpdateFrequency::EachEntity:
				if ( std::find( pass.perEntityInputsFlattened.begin( ), pass.perEntityInputsFlattened.end( ), binderIdx ) == pass.perEntityInputsFlattened.end(  ) )
				{
					pass.perEntityInputsFlattened.push_back( binderIdx );
				}
				    
				pass.perEntityInputs[ pipelineIdx ].push_back( binderIdx );
				globalResourceTable->addPerEntityResource( binderIdx );
				break;
			case UpdateFrequency::EachFrame:
				pass.perFrameInputs[ pipelineIdx ].push_back( binderIdx );
				globalResourceTable->addPerFrameResource( binderIdx );
				break;
            default: 
				break;
            }
		}

		++pipelineIdx;
	}
}

void RenderGraph::execute( )
{
	globalResourceTable->resetFrame( frameIndex );
	globalResourceTable->allocateAllPerFrameResources( frameIndex );

	frameLocks[ frameIndex ]->lock(  );
	
	for ( auto& pass : passes )
	{
		executePass( pass );
	}

	frameLocks[ frameIndex ]->unlock(  );

	if ( !redrawFrame )
	{
		frameIndex = ( frameIndex + 1 ) % renderDevice->getFrameCount( );
	}
}

void RenderGraph::executePass( const PassWrapper& pass )
{
	auto renderPass = pass.renderPass;

	renderPass->frameStart( frameIndex, pass.pipelines );

	for ( auto& output : pass.ref->outputs )
	{
        if ( std::shared_ptr< ShaderResource > & outputResource = pass.renderTargets[ frameIndex ]->outputImageMap[ output.outputResourceName ]; outputResource != nullptr )
		{
			outputResource->prepareForUsage( ResourceUsage::RenderTarget );
		}
	}

	for ( const auto& dependency : pass.dependencies )
	{
        if ( PassWrapper & dependencyPass = passes[ passMap[ dependency ] ]; !dependencyPass.executeLocks.empty( ) )
		{
			dependencyPass.executeLocks[ frameIndex ]->wait( );
		}
	}

	auto pipelineIndex = 0;

	for ( auto& pipeline : pass.pipelines )
	{
		renderPass->bindPipeline( pipeline );

		bindDependentInputs( pass, renderPass, pipelineIndex );

		for ( auto perFrameInput : pass.perFrameInputs[ pipelineIndex ] )
		{
			renderPass->bindPerFrame( globalResourceTable->getResource( perFrameInput, frameIndex ) );
		}

		++pipelineIndex;
	}

	renderPass->begin( pass.renderTargets[ frameIndex ], { 0.0f, 0.0f, 0.0f, 1.0f } );

	for ( auto& wrapper : globalResourceTable->getGeometryList( pass.ref->inputGeometry ) )
	{
		drawEntity( pass, renderPass, wrapper );
	}

	redrawFrame = !renderPass->submit( std::vector< std::shared_ptr< IResourceLock > >( ), pass.executeLocks[ frameIndex ].get( ) );

	for ( auto& output : pass.ref->outputs )
	{
        if ( std::shared_ptr< ShaderResource > & outputResource = pass.renderTargets[ frameIndex ]->outputImageMap[ output.outputResourceName ]; outputResource != nullptr ) 
		{
			outputResource->prepareForUsage( ResourceUsage::ShaderInputSampler2D );
		}
	}
}

void RenderGraph::drawEntity( const PassWrapper &pass, const std::shared_ptr< IRenderPass >& renderPass, const EntityWrapper & wrapper ) const
{
	globalResourceTable->allocatePerEntityResources( frameIndex, wrapper.entity, pass.perEntityInputsFlattened );

	auto selectedPipelines = pass.ref->selectPipeline( wrapper.entity );

	for ( auto selectedPipeline : selectedPipelines )
	{
		renderPass->bindPipeline( pass.pipelines[ selectedPipeline ] );

		for ( const int& resourceIdx : pass.perEntityInputs[ selectedPipeline ] )
		{
			renderPass->bindPerObject( globalResourceTable->getResource( resourceIdx, frameIndex ) );
		}

		for ( auto& [ ignored, resources, boundResources, subMeshGeometry ] : wrapper.subGeometries )
		{
			for ( const int& resourceIdx : pass.loadOnceInputs[ selectedPipeline ] )
			{
				renderPass->bindPerObject( boundResources[ resourceIdx ].ref );
			}

			for ( const auto& resource : resources )
			{
				renderPass->bindPerObject( resource.ref );
			}

		    uint32_t instanceCount = 1;

		    if ( auto instances = wrapper.entity->getComponent< ECS::CInstances >( ); instances != nullptr )
		    {
			    instanceCount += instances->transforms.size( );
		    }

		    renderPass->draw( instanceCount );
		}
	}
}

void RenderGraph::bindDependentInputs( const PassWrapper& pass, std::shared_ptr< IRenderPass >& renderPass, int pipelineIndex )
{
	for ( auto& input : pass.passDependentInputs[ pipelineIndex ] )
	{
        if ( auto find = pipelineInputOutputDependencies.find( input ); find != pipelineInputOutputDependencies.end( ) )
		{
			PassWrapper& inputPass = passes[ passMap[ find->second ] ];
			renderPass->bindPerFrame( inputPass.renderTargets[ frameIndex ]->outputImageMap[ input ] );
		}
	}
}

void RenderGraph::addEntity( ECS::IGameEntity * entity ) const
{
	globalResourceTable->addEntity( entity );
}

void RenderGraph::updateEntity( ECS::IGameEntity * entity ) const
{
	globalResourceTable->updateEntity( entity );
}

void RenderGraph::removeEntity( ECS::IGameEntity * entity ) const
{
	globalResourceTable->removeEntity( entity );
}

RenderGraph::~RenderGraph( )
{
	globalResourceTable.reset( );

	for ( auto& pass : passes )
	{
		for ( auto& lock : pass.executeLocks )
		{
			lock->cleanup( );
		}

		for ( auto& pipeline : pass.pipelines )
		{
			pipeline->cleanup( );
		}

		for ( auto& renderTarget : pass.renderTargets )
		{
			renderTarget->cleanup( );
		}

		pass.renderPass->cleanup( );
	}
}


END_NAMESPACES

