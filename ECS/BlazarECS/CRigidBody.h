#pragma once

#include <BlazarCore/Common.h>
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

// Initialize using BlazarEngine::Physics::CollisionShapeInitializer unless a more custom use case is required.
struct CRigidBody : public IComponent
{
public:
    std::shared_ptr< btCollisionShape > collisionShape = nullptr;
    std::shared_ptr< btMotionState > motionState = nullptr;

    float mass = 0.0f;
    std::shared_ptr< btRigidBody > instance = nullptr;

    BLAZAR_COMPONENT( CRigidBody )
};

END_NAMESPACES