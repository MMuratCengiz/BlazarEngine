#pragma once

#include "../ECS/IGameEntity.h"
#include "../ECS/CMesh.h"
#include "../ECS/CMaterial.h"
#include "../ECS/CTransform.h"

namespace Sample {

class SampleTrafficCone : public BlazarEngine::ECS::IGameEntity {
public:
    SampleTrafficCone( ) {
        auto mesh = createComponent< BlazarEngine::ECS::CMesh >( );
        mesh->path = PATH( "/assets/models/Traffic Cone.FBX" );

        auto texture = createComponent< BlazarEngine::ECS::CMaterial >( );
        auto &texInfo = texture->textures.emplace_back( BlazarEngine::ECS::Material::TextureInfo { } );
        texInfo.path = "/assets/textures/Traffic Cone UV Fixed.png";

        auto transform = createComponent< BlazarEngine::ECS::CTransform >( );
        transform->position = glm::vec3( 1.0f, 0.1f, -0.8f );
        transform->scale = glm::vec3( 0.015f, 0.015f, 0.015f );
        transform->rotation.euler = glm::vec3( -90.0f, 0.0f, 0.0f );
    }
};

}
