#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>
#include <BlazarGraphics/BuiltinPrimitives.h>
#include <BlazarPhysics/CollisionShapeInitializer.h>

using namespace BlazarEngine;

namespace Sample
{

class SampleSmallCrate : public ECS::IGameEntity
{
public:
    SampleSmallCrate( )
    {
        auto mesh = createComponent< ECS::CMesh >( );
        mesh->path = Graphics::BuiltinPrimitives::getPrimitivePath( Graphics::PrimitiveType::LightedCube );

        auto texture = createComponent< ECS::CMaterial >( );
        auto &texInfo = texture->textures.emplace_back( ECS::Material::TextureInfo { } );
        texInfo.path = "/assets/textures/container.jpg";

        auto transform = createComponent< ECS::CTransform >( );
        transform->position = glm::vec3( -0.6f, 1.0f, 5.4f );
        transform->scale = glm::vec3( 0.2f );
        transform->rotation.euler = glm::vec3( 46.0f, 0.0f, 0.0f );

        auto rigidBody = createComponent< ECS::CRigidBody >( );
        rigidBody->mass = 1.0f;

        Physics::CollisionShapeInitializer( rigidBody, transform ).initializeBoxCollisionShape( transform->scale );
    }
};

}