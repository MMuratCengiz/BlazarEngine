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

#include "PhysicsWorld.h"

NAMESPACES( ENGINE_NAMESPACE, Physics )

const float PhysicsWorld::GRAVITY_EARTH = -9.80665;

PhysicsWorld::PhysicsWorld( const PhysicsWorldConfiguration &physicsWorldConfiguration )
{
    collisionConfiguration = new btDefaultCollisionConfiguration{ };
    collisionDispatcher = new btCollisionDispatcher{ collisionConfiguration };
    overlappingPairCache = new btDbvtBroadphase{ };
    sicSolver = new btSequentialImpulseConstraintSolver{ };

    dynamicsWorld = std::make_unique< btDiscreteDynamicsWorld >( collisionDispatcher, overlappingPairCache, sicSolver, collisionConfiguration );
    dynamicsWorld->setGravity( btVector3( 0.0f, GRAVITY_EARTH, 0.0f ) );
}

void PhysicsWorld::addOrUpdateEntity( const std::shared_ptr< ECS::IGameEntity > &entity )
{
    std::shared_ptr< ECS::CTransform > transformObject = entity->getComponent< ECS::CTransform >( );
    std::shared_ptr< ECS::CCollisionObject > collisionObject = entity->getComponent< ECS::CCollisionObject >( );
    std::shared_ptr< ECS::CRigidBody > rigidBody = entity->getComponent< ECS::CRigidBody >( );

    FUNCTION_BREAK( rigidBody == nullptr && collisionObject == nullptr )

    if ( rigidBody != nullptr )
    {
        rigidBody->instance->setUserPointer( transformObject.get( ) );
        dynamicsWorld->addRigidBody( rigidBody->instance.get( ) );
    }
    if ( collisionObject != nullptr )
    {
        collisionObject->instance->setUserPointer( transformObject.get( ) );
        dynamicsWorld->addCollisionObject( collisionObject->instance.get( ) );
    }
}

void PhysicsWorld::update( const std::shared_ptr< ECS::IGameEntity > &entity )
{

}

void PhysicsWorld::tick( )
{
    dynamicsWorld->stepSimulation( Core::Time::getDeltaTime() );

    btCollisionObjectArray &collisionObjects = dynamicsWorld->getCollisionObjectArray( );

    for ( uint32_t i = 0; i < dynamicsWorld->getNumCollisionObjects( ); ++i )
    {
        btCollisionObject *&collisionObject = collisionObjects[ i ];

        btRigidBody *rigidBody = btRigidBody::upcast( collisionObject );
        btTransform transform;

        if ( rigidBody && rigidBody->getMotionState( ) )
        {
            rigidBody->getMotionState( )->getWorldTransform( transform );
        }
        else
        {
            transform = collisionObject->getWorldTransform( );
        }

        auto *blazarTransform = reinterpret_cast< ECS::CTransform * >( collisionObject->getUserPointer( ) );

        blazarTransform->position = Core::Utilities::toGlm( transform.getOrigin( ) );
        blazarTransform->rotation.euler = Core::Utilities::quatToEulerGlm( transform.getRotation( ) );

        if ( blazarTransform->rotation.rotationUnit == ECS::RotationUnit::Degrees )
        {
            blazarTransform->rotation.euler = glm::vec3(
                    glm::degrees( blazarTransform->rotation.euler.x ),
                    glm::degrees( blazarTransform->rotation.euler.y ),
                    glm::degrees( blazarTransform->rotation.euler.z )
            );
        }
    }

    dynamicsWorld->debugDrawWorld();
}

PhysicsWorld::~PhysicsWorld( )
{
    btCollisionObjectArray &collisionObjects = dynamicsWorld->getCollisionObjectArray( );

    for ( int i = dynamicsWorld->getNumCollisionObjects( ) - 1; i >= 0 ; i-- )
    {
        dynamicsWorld->removeCollisionObject( collisionObjects[ i ] );
    }

    delete sicSolver;
}

END_NAMESPACES
