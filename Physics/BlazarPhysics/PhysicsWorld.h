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
    btDefaultCollisionConfiguration *collisionConfiguration;
    btCollisionDispatcher *collisionDispatcher;
    btDbvtBroadphase *overlappingPairCache;
    btSequentialImpulseConstraintSolver *sicSolver;

    std::unique_ptr< btDiscreteDynamicsWorld > dynamicsWorld;
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