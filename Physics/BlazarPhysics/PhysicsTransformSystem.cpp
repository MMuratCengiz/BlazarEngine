//
// Created by Murat on 12/6/2020.
//

#include "PhysicsTransformSystem.h"

NAMESPACES( ENGINE_NAMESPACE, Physics )

PhysicsTransformSystem::PhysicsTransformSystem( std::shared_ptr< PhysicsWorld > physicsWorld ) : physicsWorld( std::move( physicsWorld ) )
{

}

void PhysicsTransformSystem::translate( const std::shared_ptr< ECS::IGameEntity > &entity, const glm::vec3 &translation )
{
    std::shared_ptr< ECS::CTransform > transform = entity->getComponent< ECS::CTransform >( );
    std::shared_ptr< ECS::CRigidBody > rigidBody = entity->getComponent< ECS::CRigidBody >( );

    FUNCTION_BREAK( transform == nullptr && rigidBody == nullptr )

    rigidBody->instance->applyTorque( Core::Utilities::toBt( translation ) * 70 );
    rigidBody->instance->activate( );
}

void PhysicsTransformSystem::rotate( const std::shared_ptr< ECS::IGameEntity > &entity, glm::vec3 rotation )
{
    std::shared_ptr< ECS::CTransform > transform = entity->getComponent< ECS::CTransform >( );
    std::shared_ptr< ECS::CRigidBody > rigidBody = entity->getComponent< ECS::CRigidBody >( );

    FUNCTION_BREAK( transform == nullptr && rigidBody == nullptr )
}

END_NAMESPACES
