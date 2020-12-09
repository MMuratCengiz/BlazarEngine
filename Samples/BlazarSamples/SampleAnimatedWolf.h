#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>

namespace Sample
{

class SampleAnimatedWolf : public BlazarEngine::ECS::IGameEntity
{
public:
    SampleAnimatedWolf( )
    {
        auto mesh = createComponent< BlazarEngine::ECS::CMesh >( );
        mesh->path = PATH( "/assets/models/wolf.blend" );

        auto texture = createComponent< BlazarEngine::ECS::CMaterial >( );
        auto &texInfo = texture->textures.emplace_back( BlazarEngine::ECS::Material::TextureInfo { } );
        texInfo.path = "/assets/textures/Wolf_Body.hpg";

        auto transform = createComponent< BlazarEngine::ECS::CTransform >( );
        transform->position = glm::vec3( 1.0f, 0.2f, -4.8f );
        transform->scale = glm::vec3( 2.0f );
        transform->rotation.euler = glm::vec3( -90.0f, 1.0f, 1.0f );
    }
};

}