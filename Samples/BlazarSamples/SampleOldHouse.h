#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>

namespace Sample
{

class SampleOldHouse : public BlazarEngine::ECS::IGameEntity
{
public:
    explicit SampleOldHouse(  Scene::World * world )
    {
        auto meshEntities = world->getAssetManager( )->createEntity( PATH( "/assets/models/old_house.obj") );

        for ( auto& child: meshEntities->getChildren( ) )
        {
            auto &texInfo = child->getComponent< BlazarEngine::ECS::CMaterial >( )->textures.emplace_back( BlazarEngine::ECS::Material::TextureInfo { } );
            texInfo.path = "/assets/textures/old_house_body.jpg";
        }

        addChild( meshEntities );
        auto transform = createComponent< BlazarEngine::ECS::CTransform >( );
        transform->position = glm::vec3( -5.5f, 0.15f, 5.0f );
        transform->scale = glm::vec3( 0.02f, 0.02f, 0.02f );
        transform->rotation.euler = glm::vec3( 0.0f, 0.0f, 0.0f );
    }
};

}