#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>

namespace Sample
{

class SampleCar2 : public BlazarEngine::ECS::IGameEntity
{
public:
    explicit SampleCar2( Scene::World * world )
    {
        auto meshEntities = world->getAssetManager( )->createEntity( PATH( "/assets/models/car_2.fbx" ) );

        for ( auto& child: meshEntities->getChildren( ) )
        {
            auto &texInfo = child->getComponent< BlazarEngine::ECS::CMaterial >( )->textures.emplace_back( BlazarEngine::ECS::Material::TextureInfo { } );
            texInfo.path = "/assets/textures/Car Texture 2.png";
        }

        addChild( meshEntities );

        auto transform = createComponent< BlazarEngine::ECS::CTransform >( );
        transform->position = glm::vec3( -5.5f, 0.15f, -2.8f );
        transform->scale = glm::vec3( 0.5f, 0.5f, 0.5f );
        transform->rotation.euler = glm::vec3( 0.0f, 45.0f, 0.0f );
    }
};

}