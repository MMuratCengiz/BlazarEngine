#include "RenderGraph.h"

#include <utility>

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

    passMap[ wrapper.ref->name ] = wrapper;
}

void RenderGraph::buildGraph( )
{
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

    if ( pass.adaptedInputs.empty() )
    {
        pass.adaptedInputs.resize( pass.ref->pipeLineInputs.size( ) );

        for ( const auto &pipelineInput: pass.ref->pipeLineInputs )
        {
            for ( auto &input: pipelineInput )
            {
                if ( input == StaticVars::getInputName( StaticVars::Input::GeometryData ) )
                {
                    pass.usesGeometryData = true;
                }
                else
                {
                    if ( input != StaticVars::getInputName( StaticVars::Input::Material ) )
                    {
                        pass.adaptedInputs[ pipelineIndex ].push_back( input );
                    }

                    globalResourceTable->allocateResource( input, frameIndex );
                }
            }

            pipelineIndex++;
        }
    }
    else
    {
        for ( const auto& passInputs: pass.adaptedInputs )
        {
            for ( const auto& input: passInputs )
            {
                globalResourceTable->allocateResource( input, frameIndex );
            }
        }
    }
    // --

    if ( pass.renderPass == nullptr )
    {
        pass.renderPass = renderDevice->getRenderPassProvider( )->createRenderPass( pass.ref->renderPassRequest );
        pass.renderPass->create( pass.ref->renderPassRequest );
    }

    if ( pass.renderTargets.empty( ) )
    {

        RenderTargetRequest renderTargetRequest { };
        renderTargetRequest.renderPass = pass.renderPass;
        renderTargetRequest.targetImages = RenderTargetImages::DepthAndStencil; // todo dynamic

        pass.renderTargets.resize( renderDevice->getFrameCount( ) );

        for ( uint32_t frame = 0; frame < pass.renderTargets.size( ); ++frame )
        {
            renderTargetRequest.frameIndex = frame;

            if ( pass.ref->outputs.empty( ) )
            {
                renderTargetRequest.type = RenderTargetType::SwapChain;
            }
            else
            {
                renderTargetRequest.type = RenderTargetType::Intermediate;
                renderTargetRequest.outputImages = pass.ref->outputs;

                for ( const auto &outputImage: pass.ref->outputs )
                {
                    globalResourceTable->createEmptyImageResource( outputImage, frameIndex );
                }
            }

            pass.renderTargets[ frame ] = renderDevice->getRenderPassProvider( )->createRenderTarget( renderTargetRequest );
        }
    }

    if ( pass.pipelines.empty() )
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

    frameIndex = ( frameIndex + 1 ) % renderDevice->getFrameCount( );
}

void RenderGraph::executePass( const PassWrapper &pass )
{
    std::vector< GeometryData > geometries;

    if ( pass.usesGeometryData )
    {
        geometries = globalResourceTable->getGeometryList( );
    }
    else
    {
        // geometries = globalResourceTable->singleQuad( );
    }

    std::shared_ptr< IRenderPass > renderPass = pass.renderPass;

    renderPass->frameStart( frameIndex, pass.pipelines );

    for ( auto &output: pass.ref->outputs )
    {
        globalResourceTable->prepareResource( output.outputResourceName, ResourceUsage::RenderTarget, frameIndex );
    }

    for ( const auto &dependency: pass.dependencies )
    {
        passMap[ dependency ].executeLocks[ frameIndex ]->wait( );
    }

    auto pipelineIndex = 0;

    for ( auto &pipeline: pass.pipelines )
    {
        renderPass->bindPipeline( pipeline );

        for ( auto &input: pass.adaptedInputs[ pipelineIndex ] )
        {
            renderPass->bindPerFrame( globalResourceTable->getResource( input, frameIndex ) );
        }

        ++pipelineIndex;
    }

    renderPass->begin( pass.renderTargets[ frameIndex ], { 0.0f, 0.0f, 0.0f, 1.0f } );

    for ( auto &geometry: geometries )
    {
        int selectedPipeline = pass.ref->selectPipeline( geometry.referenceEntity );
        renderPass->bindPipeline( pass.pipelines[ selectedPipeline ] );

        for ( auto &texture: geometry.textures )
        {
            renderPass->bindPerObject( texture );
        }

        renderPass->bindPerObject( geometry.vertices );

        if ( geometry.indices != nullptr )
        {
            renderPass->bindPerObject( geometry.indices );
        }
        if ( geometry.material != nullptr )
        {
            renderPass->bindPerObject( geometry.material );
        }

        globalResourceTable->setActiveGeometryModel( geometry );

        for ( auto &input: pass.adaptedInputs[ selectedPipeline ] )
        {
            renderPass->bindPerObject( globalResourceTable->getResource( input, frameIndex ) );
        }
        renderPass->draw( );
    }

    renderPass->submit( std::vector< std::shared_ptr< IResourceLock > >( ), pass.executeLocks[ frameIndex ] );

    for ( auto &output: pass.ref->outputs )
    {
        globalResourceTable->prepareResource( output.outputResourceName, ResourceUsage::ShaderInputSampler2D, frameIndex );
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
}


END_NAMESPACES

