#pragma once

#include <BlazarCore/Common.h>
#include "PhysicsWorld.h"

NAMESPACES( ENGINE_NAMESPACE, Physics )

class PhysicsTransformSystem
{
private:
    std::shared_ptr< PhysicsWorld > physicsWorld;
public:
    explicit PhysicsTransformSystem( std::shared_ptr< PhysicsWorld > physicsWorld );
    void translate( const std::shared_ptr< ECS::IGameEntity > &entity, const glm::vec3 &translation );
    void rotate( const std::shared_ptr< ECS::IGameEntity > &entity, glm::vec3 rotation );
};


END_NAMESPACES
