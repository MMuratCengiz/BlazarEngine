#pragma once

#include "../ECS/IGameEntity.h"
#include "../ECS/CMesh.h"
#include "../ECS/CMaterial.h"
#include "../ECS/CTransform.h"

namespace Sample {

class SampleCar1 : public SomeVulkan::ECS::IGameEntity {
public:
    SampleCar1( ) {
        auto mesh = createComponent< SomeVulkan::ECS::CMesh >( );
        mesh->path = PATH( "/assets/models/car_1.fbx" );

        auto texture = createComponent< SomeVulkan::ECS::CMaterial >( );
        auto &texInfo = texture->textures.emplace_back( SomeVulkan::ECS::Material::TextureInfo { } );
        texInfo.path = "/assets/textures/Car Texture 1.png";

        auto transform = createComponent< SomeVulkan::ECS::CTransform >( );
        transform->position = glm::vec3( 3.0f, 0.15f, 5.0f );
        transform->scale = glm::vec3( 0.5f, 0.5f, 0.5f );
        transform->rotation.euler = glm::vec3( -0.0f, -45.0f, 0.0f );
    }
};

}