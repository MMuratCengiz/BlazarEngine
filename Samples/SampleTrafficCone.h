#pragma once

#include "../ECS/IGameEntity.h"
#include "../ECS/CMesh.h"
#include "../ECS/CMaterial.h"
#include "../ECS/CTransform.h"

namespace Sample {

class SampleTrafficCone : public SomeVulkan::ECS::IGameEntity {
public:
    SampleTrafficCone( ) {
        auto mesh = createComponent< SomeVulkan::ECS::CMesh >( );
        mesh->path = PATH( "/assets/models/Traffic Cone.FBX" );

        auto texture = createComponent< SomeVulkan::ECS::CMaterial >( );
        auto &texInfo = texture->textures.emplace_back( SomeVulkan::ECS::Material::TextureInfo { } );
        texInfo.path = "/assets/textures/Traffic Cone UV Fixed.png";

        auto transform = createComponent< SomeVulkan::ECS::CTransform >( );
        transform->position = glm::vec3( 1.0f, 0.1f, -0.8f );
        transform->scale = glm::vec3( 0.015f, 0.015f, 0.015f );
        transform->rotation.euler = glm::vec3( -90.0f, 0.0f, 0.0f );
    }
};

}
