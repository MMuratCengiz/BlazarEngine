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

#include "CollisionShapeInitializer.h"

NAMESPACES( ENGINE_NAMESPACE, Physics )

CollisionShapeInitializer::CollisionShapeInitializer( ECS::CCollisionObject * collisionObject, ECS::CTransform * transform )
        : collisionObject( collisionObject ), transform( transform )
{
}

CollisionShapeInitializer::CollisionShapeInitializer( ECS::CRigidBody * rigidBody, ECS::CTransform * transform )
        : rigidBody( rigidBody ), transform( transform )
{
}

void CollisionShapeInitializer::initializeBoxCollisionShape( const glm::vec3 &dimensions )
{
    auto *boxShape = new btBoxShape( Core::Utilities::toBt( dimensions ) );

    shape = std::unique_ptr< btCollisionShape >( boxShape );

    initializeRequestedComponent( );
}

void CollisionShapeInitializer::initializeSphereCollisionShape( const float& radius )
{
    auto *sphereShape = new btSphereShape( radius );

    shape = std::unique_ptr< btCollisionShape >( sphereShape );

    initializeRequestedComponent( );
}

void CollisionShapeInitializer::initializeRequestedComponent( )
{
    btTransform initialTransform;
    initialTransform.setOrigin( Core::Utilities::toBt( transform->position ) );
    initialTransform.setRotation( Core::Utilities::toBtQuat( transform->rotation.euler, transform->rotation.rotationUnit == ECS::RotationUnit::Radians ) );

    // Handle the case where a rigidBody is provided
    if ( rigidBody != nullptr )
    {
        // todo, make a field of rigidBody
        btVector3 localInertia( 0.0f, 0.0f, 0.0f );

        if ( rigidBody->mass != 0 )
        {
            shape->calculateLocalInertia( rigidBody->mass, localInertia );
        }

        rigidBody->collisionShape = std::move( shape );
        rigidBody->motionState = std::make_unique< btDefaultMotionState >( initialTransform );

        rigidBody->instance = std::make_unique< btRigidBody >( rigidBody->mass, rigidBody->motionState.get( ), rigidBody->collisionShape.get( ), localInertia );
    }
        // Handle the case where a collisionObject is provided
    else
    {
        collisionObject->collisionShape = std::move( shape );
        collisionObject->instance = std::make_unique< btCollisionObject >( );
        collisionObject->instance->setCollisionShape( collisionObject->collisionShape.get( ) );
    }
}

END_NAMESPACES