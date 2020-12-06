#pragma once

#include "../ECS/IGameEntity.h"
#include "../ECS/CMesh.h"
#include "../ECS/CMaterial.h"
#include "../ECS/CTransform.h"

namespace Sample {

class SampleHouse : public BlazarEngine::ECS::IGameEntity {
public:
	SampleHouse( ) {
        auto mesh = createComponent< BlazarEngine::ECS::CMesh >( );
        mesh->path = PATH( "/assets/models/viking_room.obj" );

        auto texture = createComponent< BlazarEngine::ECS::CMaterial >( );
        auto &texInfo = texture->textures.emplace_back( BlazarEngine::ECS::Material::TextureInfo { } );
        texInfo.path = "/assets/textures/viking_room.png";

        auto transform = createComponent< BlazarEngine::ECS::CTransform >( );
        transform->position = glm::vec3( 0.0f, 0.4f, -0.8f );
        transform->rotation.euler = glm::vec3( -90.0f, 90.0f, 0.0f );
	}
};

}