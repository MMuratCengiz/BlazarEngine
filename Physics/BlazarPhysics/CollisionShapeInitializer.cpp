#include "CollisionShapeInitializer.h"

NAMESPACES( ENGINE_NAMESPACE, Physics )

CollisionShapeInitializer::CollisionShapeInitializer( std::shared_ptr< ECS::CCollisionObject > collisionObject, std::shared_ptr< ECS::CTransform > transform )
        : collisionObject( std::move( collisionObject ) ), transform( std::move( transform ) )
{
}

CollisionShapeInitializer::CollisionShapeInitializer( std::shared_ptr< ECS::CRigidBody > rigidBody, std::shared_ptr< ECS::CTransform > transform )
        : rigidBody( std::move( rigidBody ) ), transform( std::move( transform ) )
{
}

void CollisionShapeInitializer::initializeBoxCollisionShape( const glm::vec3 &dimensions )
{
    auto *boxShape = new btBoxShape( Core::Utilities::toBt( dimensions ) );

    shape = std::shared_ptr< btCollisionShape >( boxShape );

    initializeRequestedComponent( );
}

void CollisionShapeInitializer::initializeSphereCollisionShape( const float& radius )
{
    auto *sphereShape = new btSphereShape( radius );

    shape = std::shared_ptr< btCollisionShape >( sphereShape );

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
        rigidBody->motionState = std::make_shared< btDefaultMotionState >( initialTransform );

        rigidBody->instance = std::make_shared< btRigidBody >( rigidBody->mass, rigidBody->motionState.get( ), rigidBody->collisionShape.get( ), localInertia );
    }
        // Handle the case where a collisionObject is provided
    else
    {
        collisionObject->collisionShape = std::move( shape );
        collisionObject->instance = std::make_shared< btCollisionObject >( );
        collisionObject->instance->setCollisionShape( collisionObject->collisionShape.get( ) );
    }
}

END_NAMESPACES