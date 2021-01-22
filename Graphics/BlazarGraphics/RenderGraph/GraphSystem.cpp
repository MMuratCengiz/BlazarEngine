#include "GraphSystem.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

GraphSystem::GraphSystem( IRenderDevice *renderDevice, AssetManager *assetManager ) : renderDevice( renderDevice ), assetManager( assetManager )
{
    renderGraph = std::make_unique< RenderGraph >( this->renderDevice, this->assetManager );

    renderGraph->addPass( CommonPasses::createShadowMapPass( this->renderDevice ) );
    renderGraph->addPass( CommonPasses::createGBufferPass( this->renderDevice ) );
    renderGraph->addPass( CommonPasses::createLightingPass( this->renderDevice ) );
    renderGraph->addPass( CommonPasses::createSkyBoxPass( this->renderDevice ) );
    renderGraph->addPass( CommonPasses::createSMAAEdgePass( this->renderDevice ) );
    renderGraph->addPass( CommonPasses::createSMAABlendWeightPass( this->renderDevice ) );
    renderGraph->addPass( CommonPasses::createSMAANeighborPass( this->renderDevice ) );
    renderGraph->addPass( CommonPasses::createPresentPass( this->renderDevice ) );

    renderGraph->buildGraph( );

    Input::GlobalEventHandler::Instance( ).subscribeToEvent( Input::EventType::WindowResized, [ & ]( const Input::EventType &eventType, std::shared_ptr< Input::IEventParameters > eventParams )
    {
        auto parameters = Input::GlobalEventHandler::ToWindowResizedParameters( eventParams );
        isSystemActive = parameters->width > 0 && parameters->height > 0;
    } );
}

void GraphSystem::addEntity( const std::shared_ptr< ECS::IGameEntity > &entity )
{
    renderGraph->addEntity( entity );
}

void GraphSystem::updateEntity( const std::shared_ptr< ECS::IGameEntity > &entity )
{
    renderGraph->updateEntity( entity );
}

void GraphSystem::removeEntity( const std::shared_ptr< ECS::IGameEntity > &entity )
{
    renderGraph->removeEntity( entity );
}

void GraphSystem::frameStart( const std::shared_ptr< ECS::ComponentTable > &componentTable )
{
    renderGraph->prepare( componentTable );
}

void GraphSystem::entityTick( const std::shared_ptr< ECS::IGameEntity > &entity )
{
}

void GraphSystem::frameEnd( const std::shared_ptr< ECS::ComponentTable > &componentTable )
{
    FUNCTION_BREAK( !isSystemActive )

    renderGraph->execute( );
}

void GraphSystem::cleanup( )
{
    renderGraph.reset( );
}

END_NAMESPACES