#pragma once

#include <BlazarCore/Common.h>
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

// Initialize using BlazarEngine::Physics::CollisionShapeInitializer unless a more custom use case is required.
struct CCollisionObject : public IComponent
{
public:
    std::shared_ptr< btCollisionShape > collisionShape;
    std::shared_ptr< btCollisionObject > instance;

    BLAZAR_COMPONENT( CCollisionObject )
};

END_NAMESPACES