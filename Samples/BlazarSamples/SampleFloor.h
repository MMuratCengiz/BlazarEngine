#pragma once

#include "BlazarECS/ECS.h"
#include <BlazarGraphics/BuiltinPrimitives.h>
#include <BlazarPhysics/CollisionShapeInitializer.h>

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
/*
        auto tessellation = createComponent< ECS::CTessellation >( );
        getComponent< ECS::CTessellation >( )->innerLevel = 100.0f;
        getComponent< ECS::CTessellation >( )->outerLevel = 100.0f;*/

        auto material = createComponent< ECS::CMaterial >( );
        auto &texInfo = material->textures.emplace_back( BlazarEngine::ECS::Material::TextureInfo { } );
        texInfo.path = "/assets/textures/Stone_Base_Color.tga";

//        material->heightMap.path = "/assets/textures/Stone_Height.tga";

        material->textureScaleOptions.scaleX = true;
        material->textureScaleOptions.scaleY = true;
        material->textureScaleOptions.scaleZ = true;

        auto transform = createComponent< ECS::CTransform >( );
        transform->position = glm::vec3( 0.0f, 0.0f, -2.8f );
        transform->scale = glm::vec3( 30.0f, 0.1f, 30.0f );

        auto rigidBody = createComponent< ECS::CRigidBody >( );
        rigidBody->mass = 0.0f;

        Physics::CollisionShapeInitializer( rigidBody, transform ).initializeBoxCollisionShape( transform->scale );
    }
};

}