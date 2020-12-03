#pragma once

#include "../Core/Common.h"
#include "../ECS/IGameEntity.h"
#include "../ECS/ISystem.h"
#include "../ECS/CAmbientLight.h"
#include "../ECS/CDirectionalLight.h"
#include "../ECS/CPointLight.h"
#include "../ECS/CSpotLight.h"
#include "../ECS/ISystem.h"
#include "../ECS/ISystem.h"
#include "Camera.h"

NAMESPACES( SomeVulkan, Scene )

class Scene {
private:
    std::vector< std::shared_ptr< ECS::IGameEntity > > entities;
    std::shared_ptr< Camera > activeCamera;

    std::vector< ECS::CAmbientLight > ambientLights;
    std::vector< ECS::CDirectionalLight > directionalLights;
    std::vector< ECS::CPointLight > pointLights;
    std::vector< ECS::CSpotLight > spotLights;
public:
    explicit Scene( std::shared_ptr< Camera > initialCamera ) : activeCamera( std::move( initialCamera ) ) { }

    inline void setActiveCamera( std::shared_ptr< Camera > camera ) {
        activeCamera = std::move( camera );
    }

    inline void addEntity( std::shared_ptr< ECS::IGameEntity > entity ) {
        NOT_NULL( entity );
        entities.emplace_back( std::move( entity ) );
    }

    inline void addAmbientLight( ECS::CAmbientLight ambientLight ) {
        ambientLights.push_back( ambientLight );
    }

    const std::vector< ECS::CAmbientLight >& getAmbientLights( ) const {
        return ambientLights;
    }

    inline void addDirectionalLight( ECS::CDirectionalLight directionalLight ) {
        directionalLights.push_back( directionalLight );
    }

    const std::vector< ECS::CDirectionalLight >& getDirectionalLights( ) const  {
        return directionalLights;
    }

    inline void addPointLight( ECS::CPointLight pointLight ) {
        pointLights.push_back( pointLight );
    }

    const std::vector< ECS::CPointLight >& getPointLights( ) const {
        return pointLights;
    }

    inline void addSpotLight( ECS::CSpotLight spotLight ) {
        spotLights.push_back( spotLight );
    }

    const std::vector< ECS::CSpotLight >& getSpotLights( ) const {
        return spotLights;
    }

    [[nodiscard]] const std::vector< std::shared_ptr< ECS::IGameEntity > >& getEntities( ) const {
        return entities;
    }

    [[nodiscard]] const std::shared_ptr< Camera >& getCamera( ) const {
        return activeCamera;
    }
};

END_NAMESPACES
