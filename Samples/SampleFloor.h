#pragma once

#include "../ECS.h"
#include "../Graphics/BuiltinPrimitives.h"
#include "../Physics/CollisionShapeInitializer.h"

using namespace BlazarEngine;

namespace Sample
{

class SampleFloor : public ECS::IGameEntity
{
public:
    SampleFloor( )
    {
        auto mesh = createComponent< ECS::CMesh >( );
        mesh->path = Graphics::BuiltinPrimitives::getPrimitivePath( Graphics::PrimitiveType::LightedCube );

        auto texture = createComponent< ECS::CMaterial >( );
        auto &texInfo = texture->textures.emplace_back( BlazarEngine::ECS::Material::TextureInfo { } );
        texInfo.path = "/assets/textures/floor2.png";

        auto transform = createComponent< ECS::CTransform >( );
        transform->position = glm::vec3( 0.0f, 0.0f, -2.8f );
        transform->scale = glm::vec3( 30.0f, 0.1f, 30.0f );

        auto rigidBody = createComponent< ECS::CRigidBody >( );
        rigidBody->mass = 0.0f;

        Physics::CollisionShapeInitializer( rigidBody, transform ).initializeBoxCollisionShape( transform->scale );
    }
};

}