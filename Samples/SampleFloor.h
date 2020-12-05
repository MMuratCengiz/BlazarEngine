#pragma once

#include "../ECS/IGameEntity.h"
#include "../ECS/CMesh.h"
#include "../ECS/CMaterial.h"
#include "../ECS/CTransform.h"
#include "../Graphics/BuiltinPrimitives.h"

using namespace BlazarEngine;

namespace Sample {

class SampleFloor : public ECS::IGameEntity {
public:
    SampleFloor( ) {
        auto mesh = createComponent< ECS::CMesh >( );
        mesh->path = Graphics::BuiltinPrimitives::getPrimitivePath( Graphics::PrimitiveType::LightedCube );

        auto texture = createComponent< ECS::CMaterial >( );
        auto &texInfo = texture->textures.emplace_back( SomeVulkan::ECS::Material::TextureInfo { } );
        texInfo.path = "/assets/textures/floor2.png";

        auto transform = createComponent< ECS::CTransform >( );
        transform->position = glm::vec3( 0.0f, 0.0f, -2.8f );
        transform->scale = glm::vec3( 30.0f, 0.1f, 30.0f );
    }
};

}