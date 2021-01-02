//
// Created by Murat on 12/6/2020.
//

#include "PhysicsTransformSystem.h"

NAMESPACES( ENGINE_NAMESPACE, Physics )

PhysicsTransformSystem::PhysicsTransformSystem( PhysicsWorld *physicsWorld ) : physicsWorld( physicsWorld )
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

void PhysicsTransformSystem::setPositionRecursive( ECS::IGameEntity *entity, const glm::vec3 &position )
{
    std::shared_ptr< ECS::CTransform > transform = entity->getComponent< ECS::CTransform >( );
    std::shared_ptr< ECS::CRigidBody > rigidBody = entity->getComponent< ECS::CRigidBody >( );

    FUNCTION_BREAK( transform == nullptr )

    transform->position = position;

    for ( auto &child: entity->getChildren( ) )
    {
        PhysicsTransformSystem::setPositionRecursive( child.get( ), position );
        if ( rigidBody != nullptr )
        {
            rigidBody->instance->setWorldTransform( toBtTransform( transform ) );
        }
    }
}

void PhysicsTransformSystem::setRotationRecursive( ECS::IGameEntity *entity, const ECS::Rotation &rotation )
{
    std::shared_ptr< ECS::CTransform > transform = entity->getComponent< ECS::CTransform >( );
    std::shared_ptr< ECS::CRigidBody > rigidBody = entity->getComponent< ECS::CRigidBody >( );

    FUNCTION_BREAK( transform == nullptr )

    transform->rotation = rotation;

    for ( auto &child: entity->getChildren( ) )
    {
        PhysicsTransformSystem::setRotationRecursive( child.get( ), rotation );
        if ( rigidBody != nullptr )
        {
            rigidBody->instance->setWorldTransform( toBtTransform( transform ) );
        }
    }
}

void PhysicsTransformSystem::setScaleRecursive( ECS::IGameEntity *entity, const glm::vec3 &scale )
{
    std::shared_ptr< ECS::CTransform > transform = entity->getComponent< ECS::CTransform >( );

    FUNCTION_BREAK( transform == nullptr )

    transform->scale = scale;

    for ( auto &child: entity->getChildren( ) )
    {
        PhysicsTransformSystem::setScaleRecursive( child.get( ), scale );
    }
}

btTransform PhysicsTransformSystem::toBtTransform( const std::shared_ptr< ECS::CTransform > &transform )
{
    btTransform btTransform { };

    btTransform.setOrigin( Core::Utilities::toBt( transform->position ) );
    btTransform.setRotation( Core::Utilities::toBtQuat( transform->rotation.euler, transform->rotation.rotationUnit == ECS::RotationUnit::Radians ) );

    return btTransform;
}

END_NAMESPACES
