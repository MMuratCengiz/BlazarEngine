#pragma once

#include "../Core/Common.h"
#include "../ECS/IGameEntity.h"
#include "../ECS/ISystem.h"
#include "Camera.h"

NAMESPACES( SomeVulkan, Scene )

class Scene {
private:
    std::vector< std::shared_ptr< ECS::IGameEntity > > entities;
    std::shared_ptr< Camera > activeCamera;
public:
    explicit Scene( std::shared_ptr< Camera > initialCamera ) : activeCamera( std::move( initialCamera ) ) { }

    void setActiveCamera( std::shared_ptr< Camera > camera ) {
        activeCamera = std::move( camera );
    }

    void addEntity( std::shared_ptr< ECS::IGameEntity > entity ) {
        entities.emplace_back( std::move( entity ) );
    }

    [[nodiscard]] const std::vector< std::shared_ptr< ECS::IGameEntity > >& getEntities( ) const {
        return entities;
    }

    [[nodiscard]] const std::shared_ptr< Camera >& getCamera( ) const {
        return activeCamera;
    }
};

END_NAMESPACES
