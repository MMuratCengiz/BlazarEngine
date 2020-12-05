#pragma once

#include "../ECS/IGameEntity.h"
#include "../ECS/CMesh.h"
#include "../ECS/CMaterial.h"
#include "../ECS/CTransform.h"

namespace Sample {

class SampleOldHouse : public BlazarEngine::ECS::IGameEntity {
public:
    SampleOldHouse( ) {
        auto mesh = createComponent< BlazarEngine::ECS::CMesh >( );
        mesh->path = PATH( "/assets/models/old_house.obj" );
        mesh->cullMode = SomeVulkan::ECS::CullMode::None;

        auto texture = createComponent< BlazarEngine::ECS::CMaterial >( );
        auto &texInfo = texture->textures.emplace_back( SomeVulkan::ECS::Material::TextureInfo { } );
        texInfo.path = "/assets/textures/old_house_body.jpg";

        auto transform = createComponent< BlazarEngine::ECS::CTransform >( );
        transform->position = glm::vec3( -5.5f, 0.15f, 5.0f );
        transform->scale = glm::vec3( 0.02f, 0.02f, 0.02f );
        transform->rotation.euler = glm::vec3( 0.0f, 0.0f, 0.0f );
	}
};

}