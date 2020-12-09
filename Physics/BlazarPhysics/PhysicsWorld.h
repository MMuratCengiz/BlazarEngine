#pragma once

#include <BlazarCore/Utilities.h>
#include "BlazarECS/ECS.h"

NAMESPACES( ENGINE_NAMESPACE, Physics )

struct PhysicsWorldConfiguration
{

};

class PhysicsWorld
{
private:
    std::shared_ptr< btDefaultCollisionConfiguration > collisionConfiguration;
    std::shared_ptr< btCollisionDispatcher > collisionDispatcher;
    std::shared_ptr< btDbvtBroadphase > overlappingPairCache;
    std::shared_ptr< btSequentialImpulseConstraintSolver > sicSolver;
    std::shared_ptr< btDiscreteDynamicsWorld > dynamicsWorld;
    std::vector< ECS::CTransform > collisionTransforms;
public:
    static const float GRAVITY_EARTH;

    explicit PhysicsWorld( const PhysicsWorldConfiguration &physicsWorldConfiguration );
    void addOrUpdateEntity( const std::shared_ptr< ECS::IGameEntity > &entity );
    void update( const std::shared_ptr< ECS::IGameEntity > &entity );
    void tick( );

    ~PhysicsWorld();
};

END_NAMESPACES