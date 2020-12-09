#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>
#include <BlazarGraphics/BuiltinPrimitives.h>
#include <BlazarPhysics/CollisionShapeInitializer.h>

using namespace BlazarEngine;

namespace Sample
{

class SampleBall : public ECS::IGameEntity
{
public:
    SampleBall( )
    {
        auto mesh = createComponent< ECS::CMesh >( );
        mesh->path = PATH( "/assets/models/medieval house.obj" );
        mesh->cullMode = ECS::CullMode::None;

        auto texture = createComponent< ECS::CMaterial >( );
        auto &texInfo = texture->textures.emplace_back( ECS::Material::TextureInfo { } );
        texInfo.path = "/assets/textures/medieval_house.png";

        auto transform = createComponent< ECS::CTransform >( );
        transform->position = glm::vec3( 0.0f, 300.0f, -0.8f );
        transform->scale = glm::vec3( 1.0f );

        auto rigidBody = createComponent< ECS::CRigidBody >( );
        rigidBody->mass = 0.1f;

        Physics::CollisionShapeInitializer( rigidBody, transform ).initializeBoxCollisionShape( transform->scale );
    }
};

}
