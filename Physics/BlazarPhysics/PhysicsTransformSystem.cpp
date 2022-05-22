/*
Blazar Engine - 3D Game Engine
Copyright (c) 2020-2021 Muhammed Murat Cengiz

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "PhysicsTransformSystem.h"

NAMESPACES( ENGINE_NAMESPACE, Physics )

PhysicsTransformSystem::PhysicsTransformSystem( PhysicsWorld *physicsWorld ) : physicsWorld( physicsWorld )
{

}

void PhysicsTransformSystem::translate( ECS::IGameEntity * entity, const glm::vec3 &translation )
{
    ECS::CTransform * transform = entity->getComponent< ECS::CTransform >( );
    ECS::CRigidBody * rigidBody = entity->getComponent< ECS::CRigidBody >( );

    FUNCTION_BREAK( transform == nullptr && rigidBody == nullptr )

    rigidBody->instance->setLinearVelocity( rigidBody->instance->getLinearVelocity( ) + Core::Utilities::toBt( translation ) );
    rigidBody->instance->activate( );
}

void PhysicsTransformSystem::rotate( ECS::IGameEntity * entity, glm::vec3 rotation )
{
    ECS::CTransform * transform = entity->getComponent< ECS::CTransform >( );
    ECS::CRigidBody * rigidBody = entity->getComponent< ECS::CRigidBody >( );

    FUNCTION_BREAK( transform == nullptr && rigidBody == nullptr )
}

void PhysicsTransformSystem::setPositionRecursive( ECS::IGameEntity *entity, const glm::vec3 &position )
{
    ECS::CTransform * transform = entity->getComponent< ECS::CTransform >( );
    ECS::CRigidBody * rigidBody = entity->getComponent< ECS::CRigidBody >( );

    FUNCTION_BREAK( transform == nullptr )

    transform->position = position;

    for ( auto &child: entity->getChildren( ) )
    {
        PhysicsTransformSystem::setPositionRecursive( child, position );
        if ( rigidBody != nullptr )
        {
            rigidBody->instance->setWorldTransform( toBtTransform( transform ) );
        }
    }
}

void PhysicsTransformSystem::setRotationRecursive( ECS::IGameEntity *entity, const ECS::Rotation &rotation )
{
    ECS::CTransform * transform = entity->getComponent< ECS::CTransform >( );
    ECS::CRigidBody * rigidBody = entity->getComponent< ECS::CRigidBody >( );

    FUNCTION_BREAK( transform == nullptr )

    transform->rotation = rotation;

    for ( auto &child: entity->getChildren( ) )
    {
        PhysicsTransformSystem::setRotationRecursive( child, rotation );
        if ( rigidBody != nullptr )
        {
            rigidBody->instance->setWorldTransform( toBtTransform( transform ) );
        }
    }
}

void PhysicsTransformSystem::setScaleRecursive( ECS::IGameEntity *entity, const glm::vec3 &scale )
{
    ECS::CTransform * transform = entity->getComponent< ECS::CTransform >( );

    FUNCTION_BREAK( transform == nullptr )

    transform->scale = scale;

    for ( auto &child: entity->getChildren( ) )
    {
        PhysicsTransformSystem::setScaleRecursive( child, scale );
    }
}

btTransform PhysicsTransformSystem::toBtTransform( ECS::CTransform * transform )
{
    btTransform btTransform { };

    btTransform.setOrigin( Core::Utilities::toBt( transform->position ) );
    btTransform.setRotation( Core::Utilities::toBtQuat( transform->rotation.euler, transform->rotation.rotationUnit == ECS::RotationUnit::Radians ) );

    return btTransform;
}

void PhysicsTransformSystem::addInstanceRecursive( ECS::IGameEntity *entity, ECS::CTransform transform )
{
    if ( !entity->hasComponent< ECS::CInstances >( ) )
    {
        entity->createComponent< ECS::CInstances >( );
    }

    entity->getComponent< ECS::CInstances >( )->transforms.push_back( transform );

    for ( auto &child: entity->getChildren( ) )
    {
        PhysicsTransformSystem::addInstanceRecursive( child, transform );
    }
}

END_NAMESPACES
