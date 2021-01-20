#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>
#include <BlazarInput/GlobalEventHandler.h>
#include "RenderGraph.h"
#include "CommonPasses.h"
#include "../IRenderDevice.h"
#include "../AssetManager.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class GraphSystem : public ECS::ISystem
{
private:
    std::unique_ptr< RenderGraph > renderGraph;
    IRenderDevice* renderDevice;
    AssetManager* assetManager;
    bool isSystemActive = true;
public:
    GraphSystem( IRenderDevice* renderDevice, AssetManager* assetManager );
    void addEntity( const std::shared_ptr< ECS::IGameEntity > &entity ) override;
    void updateEntity( const std::shared_ptr< ECS::IGameEntity > &entity ) override;
    void removeEntity( const std::shared_ptr< ECS::IGameEntity > &entity ) override;

    void frameStart( const std::shared_ptr< ECS::ComponentTable > &componentTable ) override;
    void entityTick( const std::shared_ptr< ECS::IGameEntity > &entity ) override;
    void frameEnd( const std::shared_ptr< ECS::ComponentTable > &componentTable ) override;
    void cleanup( ) override;
};

END_NAMESPACES