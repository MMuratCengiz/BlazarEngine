#include "RenderGraph.h"

#include <utility>
#include <boost/format.hpp>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

RenderGraph::RenderGraph( IRenderDevice *renderDevice, AssetManager *assetManager ) : renderDevice( renderDevice ), assetManager( assetManager )
{
    globalResourceTable = std::make_unique< GlobalResourceTable >( this->renderDevice, this->assetManager );
}

void RenderGraph::addPass( std::shared_ptr< Pass > pass )
{
    FUNCTION_BREAK( passMap.find( pass->name ) != passMap.end( ) )

    PassWrapper &wrapper = passes.emplace_back( );
    wrapper.renderPass = nullptr;
    wrapper.ref = std::move( pass );

    passMap[ wrapper.ref->name ] = passes.size( ) - 1;
}

void RenderGraph::buildGraph( )
{
    // flatten pipeline input, saves a loop later
    for ( auto &pass: passes )
    {
        uint32_t pipelineIndex = 0;

        pass.pipelineInputsMap.resize( pass.ref->pipelineInputs.size( ) );

        for ( const auto &pipelineInputs: pass.ref->pipelineInputs )
        {
            for ( const auto &input: pipelineInputs )
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
    for ( auto &firstPass: passes )
    {
        for ( const auto &secondPass: passes )
        {
            for ( const auto &firstPassInput: firstPass.pipelineInputsFlat )
            {
                for ( const auto &secondPassOutput: secondPass.ref->outputs )
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
    for ( const auto &pass: passes )
    {
        for ( const auto &dependency: pass.dependencies )
        {
            PassWrapper &dependencyPass = passes[ passMap[ dependency ] ];
            for ( const auto &input: pass.pipelineInputsFlat )
            {
                for ( const auto &dependencyOutput: dependencyPass.ref->outputs )
                {
                    if ( input == dependencyOutput.outputResourceName )
                    {
                        this->pipelineInputOutputDependencies[ input ] = dependency;
                    }
                }
            }
        }
    }
    // Todo
}

void RenderGraph::prepare( const std::shared_ptr< ECS::ComponentTable > &componentTable )
{
    // Last pass should always be the render pass!
    PassWrapper &lastPass = passes[ passes.size( ) - 1 ];
    if ( !lastPass.executeLocks.empty( ) ) // make sure it is not the very first frame
    {
        lastPass.executeLocks[ frameIndex ]->wait( );
    }

    globalResourceTable->resetTable( componentTable, frameIndex );

    for ( auto &pass: passes )
    {
        preparePass( pass );
    }
}

void RenderGraph::preparePass( PassWrapper &pass )
{
    // Initialize inputs
    uint32_t pipelineIndex = 0;

    if ( pass.adaptedInputs.empty( ) )
    {
        pass.adaptedInputs.resize( pass.ref->pipelineInputs.size( ) );

        for ( const auto &pipelineInput: pass.ref->pipelineInputs )
        {
            for ( auto &input: pipelineInput )
            {
                if ( input == StaticVars::getInputName( StaticVars::Input::GeometryData ) )
                {
                    pass.usesGeometryData = true;
                }
                else if ( input == "ScreenQuad" )
                {
                    pass.inputGeometry = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainSquare );
                }
                else if ( input == "ScreenCube" )
                {
                    pass.inputGeometry = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainCube );
                }
                else if ( input == "ScreenOversizedTriangle" )
                {
                    pass.inputGeometry = BuiltinPrimitives::getPrimitivePath( PrimitiveType::PlainTriangle );
                }
                else
                {
                    auto bindType = globalResourceTable->getResourceBinder( )->getResourceBindTypeOptional( input );

                    if ( !bindType.has_value( ) || bindType.value( ) == ResourceBindType::PerFrameTexture || bindType.value( ) == ResourceBindType::PerFrameUniform )
                    {
                        pass.adaptedInputs[ pipelineIndex ].push_back( input );
                    }

                    if ( pipelineInputOutputDependencies.find( input ) == pipelineInputOutputDependencies.end( ) )
                    {
                        globalResourceTable->allocateResource( input, frameIndex );
                    }
                }
            }

            pipelineIndex++;
        }
    }
    else
    {
        for ( const auto &passInputs: pass.adaptedInputs )
        {
            for ( const auto &input: passInputs )
            {
                if ( pipelineInputOutputDependencies.find( input ) == pipelineInputOutputDependencies.end( ) )
                {
                    globalResourceTable->allocateResource( input, frameIndex );
                }
            }
        }
    }
    // --

    if ( pass.renderPass == nullptr )
    {
        pass.ref->renderPassRequest.outputImages = pass.ref->outputs;
        pass.renderPass = renderDevice->getRenderPassProvider( )->createRenderPass( pass.ref->renderPassRequest );
        pass.renderPass->create( pass.ref->renderPassRequest );
    }

    if ( pass.renderTargets.empty( ) )
    {
        RenderTargetRequest renderTargetRequest { };
        renderTargetRequest.renderPass = pass.renderPass;

        pass.renderTargets.resize( renderDevice->getFrameCount( ) );

        for ( uint32_t frame = 0; frame < pass.renderTargets.size( ); ++frame )
        {
            renderTargetRequest.frameIndex = frame;
            renderTargetRequest.type = RenderTargetType::Intermediate;
            renderTargetRequest.renderArea = pass.ref->renderPassRequest.renderArea;

            for ( const auto &output: pass.ref->outputs )
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
        pipelineIndex = 0;
        pass.pipelines.resize( pass.ref->pipelineRequests.size( ) );

        for ( auto &pipelineRequest: pass.ref->pipelineRequests )
        {
            pipelineRequest.parentPass = pass.renderPass;
            pass.pipelines[ pipelineIndex ] = renderDevice->getPipelineProvider( )->createPipeline( pipelineRequest );
            ++pipelineIndex;
        }
    }

    if ( pass.executeLocks.empty( ) )
    {
        pass.executeLocks = std::vector< std::shared_ptr< IResourceLock > >( renderDevice->getFrameCount( ),
                                                                             renderDevice->getResourceProvider( )->createLock( ResourceLockType::Fence ) );
    }
}

void RenderGraph::execute( )
{
    for ( auto &pass: passes )
    {
        executePass( pass );
    }

    if ( !redrawFrame )
    {
        frameIndex = ( frameIndex + 1 ) % renderDevice->getFrameCount( );
    }
}

void RenderGraph::executePass( const PassWrapper &pass )
{
    auto logEntry = boost::format( "Running pass with name: %1%." ) % pass.ref->name;
    TRACE( "RenderGraph", VERBOSITY_LOW, logEntry.str( ).c_str( ) )

    std::vector< GeometryData > geometries;

    if ( pass.usesGeometryData )
    {
        geometries = globalResourceTable->getGeometryList( );
    }
    else
    {
        geometries = globalResourceTable->getOutputGeometryList( pass.inputGeometry );
    }

    std::shared_ptr< IRenderPass > renderPass = pass.renderPass;

    renderPass->frameStart( frameIndex, pass.pipelines );

    for ( auto &output: pass.ref->outputs )
    {
        std::shared_ptr< ShaderResource > &outputResource = pass.renderTargets[ frameIndex ]->outputImageMap[ output.outputResourceName ];
        if ( outputResource != nullptr ) // Could be a non resource output
        {
            outputResource->prepareForUsage( ResourceUsage::RenderTarget );
        }
    }

    for ( const auto &dependency: pass.dependencies )
    {
        PassWrapper &dependencyPass = passes[ passMap[ dependency ] ];
        if ( !dependencyPass.executeLocks.empty( ) )
        {
            dependencyPass.executeLocks[ frameIndex ]->wait( );
        }
    }

    auto pipelineIndex = 0;

    for ( auto &pipeline: pass.pipelines )
    {
        renderPass->bindPipeline( pipeline );

        bindAdaptedInputs( pass, renderPass, pipelineIndex, true );

        ++pipelineIndex;
    }

    renderPass->begin( pass.renderTargets[ frameIndex ], { 0.0f, 0.0f, 0.0f, 1.0f } );

    for ( auto &geometry: geometries )
    {
        auto selectedPipelines = pass.ref->selectPipeline( geometry.referenceEntity );

        for ( auto selectedPipeline: selectedPipelines )
        {
            renderPass->bindPipeline( pass.pipelines[ selectedPipeline ] );

            for ( const auto &resource: geometry.resources )
            {
                if ( pass.pipelineInputsMap[ selectedPipeline ].find( resource.boundResourceName ) != pass.pipelineInputsMap[ selectedPipeline ].end( ) )
                {
                    renderPass->bindPerObject( resource.ref );
                }
            }

            if ( geometry.modelTransformPtr != nullptr )
            {
                globalResourceTable->setActiveGeometryModel( geometry );
            }

            bindAdaptedInputs( pass, renderPass, selectedPipeline, false );

            uint32_t instanceCount = 1;

            auto instances = geometry.referenceEntity->getComponent< ECS::CInstances >( );
            if ( instances != nullptr )
            {
                instanceCount += instances->transforms.size( );
            }

            renderPass->draw( instanceCount );
        }
    }

    redrawFrame = !renderPass->submit( std::vector< std::shared_ptr< IResourceLock > >( ), pass.executeLocks[ frameIndex ] );

    for ( auto &output: pass.ref->outputs )
    {
        std::shared_ptr< ShaderResource > &outputResource = pass.renderTargets[ frameIndex ]->outputImageMap[ output.outputResourceName ];
        if ( outputResource != nullptr ) // Could be a non resource output
        {
            outputResource->prepareForUsage( ResourceUsage::ShaderInputSampler2D );
        }
    }

    logEntry = boost::format( "Pass with name: %1% ended." ) % pass.ref->name;
    TRACE( "RenderGraph", VERBOSITY_LOW, logEntry.str( ).c_str( ) )
}

void RenderGraph::bindAdaptedInputs( const PassWrapper &pass, std::shared_ptr< IRenderPass > &renderPass, int pipelineIndex, const bool &bindPerFrame )
{
    for ( auto &input: pass.adaptedInputs[ pipelineIndex ] )
    {
        auto find = pipelineInputOutputDependencies.find( input );
        if ( find != pipelineInputOutputDependencies.end( ) )
        {
            PassWrapper &inputPass = passes[ passMap[ find->second ] ];
            if ( bindPerFrame )
            {
                renderPass->bindPerFrame( inputPass.renderTargets[ frameIndex ]->outputImageMap[ input ] );
            }
            else
            {
                renderPass->bindPerObject( inputPass.renderTargets[ frameIndex ]->outputImageMap[ input ] );
            }
        }
        else
        {
            if ( bindPerFrame )
            {
                renderPass->bindPerFrame( globalResourceTable->getResource( input, frameIndex ) );
            }
            else
            {
                renderPass->bindPerObject( globalResourceTable->getResource( input, frameIndex ) );
            }
        }
    }
}

void RenderGraph::addEntity( const std::shared_ptr< ECS::IGameEntity > &entity )
{
    globalResourceTable->addEntity( entity );
}

void RenderGraph::updateEntity( const std::shared_ptr< ECS::IGameEntity > &entity )
{
    globalResourceTable->updateEntity( entity );
}

void RenderGraph::removeEntity( const std::shared_ptr< ECS::IGameEntity > &entity )
{
    globalResourceTable->removeEntity( entity );
}

RenderGraph::~RenderGraph( )
{
    globalResourceTable.reset( );

    for ( auto &pass: passes )
    {
        for ( auto &lock: pass.executeLocks )
        {
            lock->cleanup( );
        }

        for ( auto &pipeline: pass.pipelines )
        {
            pipeline->cleanup( );
        }

        for ( auto &renderTarget: pass.renderTargets )
        {
            renderTarget->cleanup( );
        }

        pass.renderPass->cleanup( );
    }
}


END_NAMESPACES

