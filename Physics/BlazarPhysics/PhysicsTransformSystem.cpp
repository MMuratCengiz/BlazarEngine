//
// Created by Murat on 12/6/2020.
//

#include "PhysicsTransformSystem.h"

NAMESPACES( ENGINE_NAMESPACE, Physics )

PhysicsTransformSystem::PhysicsTransformSystem( PhysicsWorld* physicsWorld ) : physicsWorld( physicsWorld )
{

}

void PhysicsTransformSystem::translate( const std::shared_ptr< ECS::IGameEntity > &entity, const glm::vec3 &translation )
{
    std::shared_ptr< ECS::CTransform > transform = entity->getComponent< ECS::CTransform >( );
    std::shared_ptr< ECS::CRigidBody > rigidBody = entity->getComponent< ECS::CRigidBody >( );

    FUNCTION_BREAK( transform == nullptr && rigidBody == nullptr )

    rigidBody->instance->setLinearVelocity( rigidBody->instance->getLinearVelocity( ) + Core::Utilities::toBt( translation ) );
    rigidBody->instance->activate( );
}

void PhysicsTransformSystem::rotate( const std::shared_ptr< ECS::IGameEntity > &entity, glm::vec3 rotation )
{
    std::shared_ptr< ECS::CTransform > transform = entity->getComponent< ECS::CTransform >( );
    std::shared_ptr< ECS::CRigidBody > rigidBody = entity->getComponent< ECS::CRigidBody >( );

    FUNCTION_BREAK( transform == nullptr && rigidBody == nullptr )
}

END_NAMESPACES
