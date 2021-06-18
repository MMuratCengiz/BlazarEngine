#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>
#include <BlazarPhysics/PhysicsTransformSystem.h>
#include <BlazarScene/World.h>

namespace Sample
{

class SampleAnimatedFox : public BlazarEngine::ECS::IGameEntity
{
public:
    explicit SampleAnimatedFox( BlazarEngine::Scene::World * world );

    void iterChildren( const std::shared_ptr< BlazarEngine::ECS::IGameEntity >& entity );
};

}