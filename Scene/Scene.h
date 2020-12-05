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

NAMESPACES( ENGINE_NAMESPACE, Scene )

class Scene {
private:
    std::vector< std::shared_ptr< ECS::IGameEntity > > entities;
    std::shared_ptr< Camera > activeCamera;

    std::vector< std::shared_ptr< ECS::CAmbientLight > > ambientLights;
    std::vector< std::shared_ptr< ECS::CDirectionalLight > > directionalLights;
    std::vector< std::shared_ptr< ECS::CPointLight > > pointLights;
    std::vector< std::shared_ptr< ECS::CSpotLight > > spotLights;
public:
    explicit Scene( std::shared_ptr< Camera > initialCamera ) : activeCamera( std::move( initialCamera ) ) { }

    inline void setActiveCamera( std::shared_ptr< Camera > camera ) {
        activeCamera = std::move( camera );
    }

    inline void addEntity( std::shared_ptr< ECS::IGameEntity > entity ) {
        NOT_NULL( entity );
        entities.emplace_back( std::move( entity ) );
    }

    inline void addAmbientLight( const std::shared_ptr< ECS::CAmbientLight >& ambientLight ) {
        ambientLights.push_back( ambientLight );
    }

    [[nodiscard]] const std::vector< std::shared_ptr< ECS::CAmbientLight > >& getAmbientLights( ) const {
        return ambientLights;
    }

    inline void addDirectionalLight( const std::shared_ptr< ECS::CDirectionalLight >& directionalLight ) {
        directionalLights.push_back( directionalLight );
    }

    [[nodiscard]] const std::vector< std::shared_ptr< ECS::CDirectionalLight > >& getDirectionalLights( ) const  {
        return directionalLights;
    }

    inline void addPointLight( const std::shared_ptr< ECS::CPointLight >& pointLight ) {
        pointLights.push_back( pointLight );
    }

    [[nodiscard]] const std::vector< std::shared_ptr< ECS::CPointLight > >& getPointLights( ) const {
        return pointLights;
    }

    inline void addSpotLight( const std::shared_ptr< ECS::CSpotLight >& spotLight ) {
        spotLights.push_back( spotLight );
    }

    [[nodiscard]] const std::vector< std::shared_ptr< ECS::CSpotLight > >& getSpotLights( ) const {
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
