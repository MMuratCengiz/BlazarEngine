#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>
#include <BlazarPhysics/PhysicsTransformSystem.h>

namespace Sample
{

class SampleCar2 : public BlazarEngine::ECS::IGameEntity
{
public:
    explicit SampleCar2( Scene::World * world )
    {
        auto meshEntities = world->getAssetManager( )->createEntity( PATH( "/assets/models/car_2.gltf" ) );

        for ( auto& child: meshEntities->getChildren( ) )
        {
            child->getComponent< BlazarEngine::ECS::CMaterial >( )->shininess = 1.0f;
            auto &texInfo = child->getComponent< BlazarEngine::ECS::CMaterial >( )->textures.emplace_back( BlazarEngine::ECS::Material::TextureInfo { } );
            texInfo.path = "/assets/textures/Car Texture 2.png";
        }

        addChild( meshEntities );

        BlazarEngine::Physics::PhysicsTransformSystem::setPositionRecursive( this, glm::vec3( -5.5f, 0.15f, -2.8f ) );
        BlazarEngine::Physics::PhysicsTransformSystem::setRotationRecursive( this, { BlazarEngine::ECS::RotationUnit::Degrees, glm::vec3( 0.0f, 45.0f, 0.0f ) } );
        BlazarEngine::Physics::PhysicsTransformSystem::setScaleRecursive( this, glm::vec3( 0.5f, 0.5f, 0.5f ) );
    }
};

}