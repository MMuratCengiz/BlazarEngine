#pragma once

#include "../ECS/IGameEntity.h"
#include "../ECS/CMesh.h"
#include "../ECS/CMaterial.h"
#include "../ECS/CTransform.h"

namespace Sample {

class SampleCar2 : public BlazarEngine::ECS::IGameEntity {
public:
    SampleCar2( ) {
        auto mesh = createComponent< BlazarEngine::ECS::CMesh >( );
        mesh->path = PATH( "/assets/models/car_2.fbx" );

        auto texture = createComponent< BlazarEngine::ECS::CMaterial >( );
        auto &texInfo = texture->textures.emplace_back( BlazarEngine::ECS::Material::TextureInfo { } );
        texInfo.path = "/assets/textures/Car Texture 2.png";

        auto transform = createComponent< BlazarEngine::ECS::CTransform >( );
        transform->position = glm::vec3( -5.5f, 0.15f, -2.8f );
        transform->scale = glm::vec3( 0.5f, 0.5f, 0.5f );
        transform->rotation.euler = glm::vec3( 0.0f, 45.0f, 0.0f );
    }
};

}