#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>

namespace Sample
{

class SampleTrafficCone : public BlazarEngine::ECS::IGameEntity
{
public:
    explicit SampleTrafficCone( Scene::World * world )
    {
        auto meshEntities = world->getAssetManager( )->createEntity( PATH( "/assets/models/Traffic Cone.FBX" ) );

        for ( auto& child: meshEntities->getChildren( ) )
        {
            auto &texInfo = child->getComponent< BlazarEngine::ECS::CMaterial >( )->textures.emplace_back( BlazarEngine::ECS::Material::TextureInfo { } );
            texInfo.path = "/assets/textures/Traffic Cone UV Fixed.png";
        }

         addChild( meshEntities );

        auto transform = createComponent< BlazarEngine::ECS::CTransform >( );
        transform->position = glm::vec3( 1.0f, 0.1f, -0.8f );
        transform->scale = glm::vec3( 0.015f, 0.015f, 0.015f );
        transform->rotation.euler = glm::vec3( -90.0f, 0.0f, 0.0f );
    }
};

}
