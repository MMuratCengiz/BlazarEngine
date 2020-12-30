#include "GraphSystem.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

GraphSystem::GraphSystem( IRenderDevice* renderDevice, AssetManager* assetManager ) : renderDevice( renderDevice ), assetManager( assetManager )
{
    renderGraph = std::make_unique< RenderGraph >( this->renderDevice, this->assetManager );

    renderGraph->addPass( CommonPasses::createDefaultPass( this->renderDevice ) );
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
    renderGraph->execute( );
}

void GraphSystem::cleanup( )
{
    renderGraph.reset( );
}

END_NAMESPACES