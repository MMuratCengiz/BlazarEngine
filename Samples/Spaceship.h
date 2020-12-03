#pragma once

#include "../ECS/IGameEntity.h"
#include "../ECS/CMesh.h"
#include "../ECS/CMaterial.h"
#include "../ECS/CTransform.h"

namespace Sample {


class Spaceship : public SomeVulkan::ECS::IGameEntity {
public:
    Spaceship( ) {
        auto mesh = createComponent< SomeVulkan::ECS::CMesh >( );
        mesh->path = PATH( "/assets/models/spaceship.obj" );

//        auto texture = createComponent< SomeVulkan::ECS::CMaterial >( );
//        auto &texInfo = texture->textures.emplace_back( SomeVulkan::ECS::Material::TextureInfo { } );
//        texInfo.path = "/assets/textures/viking_room.png";

        auto transform = createComponent< SomeVulkan::ECS::CTransform >( );
        transform->position = glm::vec3( 0.0f, 40.4f, -15.8f );
        transform->rotation.euler = glm::vec3( -90.0f, 90.0f, 0.0f );
    }
};

}