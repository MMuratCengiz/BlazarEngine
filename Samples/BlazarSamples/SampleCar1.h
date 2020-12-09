#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>

namespace Sample
{

class SampleCar1 : public BlazarEngine::ECS::IGameEntity
{
public:
    SampleCar1( )
    {
        auto mesh = createComponent< BlazarEngine::ECS::CMesh >( );
        mesh->path = PATH( "/assets/models/car_1.fbx" );

        auto texture = createComponent< BlazarEngine::ECS::CMaterial >( );
        auto &texInfo = texture->textures.emplace_back( BlazarEngine::ECS::Material::TextureInfo { } );
        texInfo.path = "/assets/textures/Car Texture 1.png";

        auto transform = createComponent< BlazarEngine::ECS::CTransform >( );
        transform->position = glm::vec3( 3.0f, 0.15f, 5.0f );
        transform->scale = glm::vec3( 0.5f, 0.5f, 0.5f );
        transform->rotation.euler = glm::vec3( -0.0f, -45.0f, 0.0f );
    }
};

}