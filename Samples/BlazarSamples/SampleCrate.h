#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>
#include <BlazarGraphics/BuiltinPrimitives.h>
#include <BlazarPhysics/CollisionShapeInitializer.h>

using namespace BlazarEngine;

namespace Sample
{

class SampleCrate : public ECS::IGameEntity
{
public:
    SampleCrate( )
    {
        auto mesh = createComponent< ECS::CMesh >( );
        mesh->path = Graphics::BuiltinPrimitives::getPrimitivePath( Graphics::PrimitiveType::LightedCube );

        auto texture = createComponent< ECS::CMaterial >( );
        auto &texInfo = texture->textures.emplace_back( ECS::Material::TextureInfo { } );
        texInfo.path = PATH( "/assets/textures/container.jpg" );

        auto transform = createComponent< ECS::CTransform >( );
        transform->position = glm::vec3( 0.0f, 200.0f, -2.8f );
        transform->scale = glm::vec3( 1.0f );
        transform->rotation.euler = glm::vec3( 46.0f, 0.0f, 0.0f );

        auto rigidBody = createComponent< ECS::CRigidBody >( );
        rigidBody->mass = 0.2f;

        Physics::CollisionShapeInitializer( rigidBody, transform ).initializeBoxCollisionShape( transform->scale );
    }
};

}