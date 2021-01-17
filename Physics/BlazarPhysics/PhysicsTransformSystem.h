#pragma once

#include <BlazarCore/Common.h>
#include "PhysicsWorld.h"

NAMESPACES( ENGINE_NAMESPACE, Physics )

class PhysicsTransformSystem
{
private:
    PhysicsWorld* physicsWorld;
public:
    explicit PhysicsTransformSystem( PhysicsWorld* physicsWorld );
    // Care, bypasses physics system, ideally only used in initialization
    static void setPositionRecursive( ECS::IGameEntity *entity, const glm::vec3 &position );
    // Care, bypasses physics system, ideally only used in initialization
    static void setRotationRecursive( ECS::IGameEntity *entity, const ECS::Rotation &rotation );
    // Care, bypasses physics system, ideally only used in initialization
    static void setScaleRecursive( ECS::IGameEntity *entity, const glm::vec3 &scale );

    static void addInstanceRecursive( ECS::IGameEntity *entity, std::shared_ptr< ECS::CTransform > transform );

    void translate( const std::shared_ptr< ECS::IGameEntity > &entity, const glm::vec3 &translation );
    void rotate( const std::shared_ptr< ECS::IGameEntity > &entity, glm::vec3 rotation );
private:
    static btTransform toBtTransform( const std::shared_ptr< ECS::CTransform >& transform );
};


END_NAMESPACES
