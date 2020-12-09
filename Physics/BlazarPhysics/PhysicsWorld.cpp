#include "PhysicsWorld.h"

NAMESPACES( ENGINE_NAMESPACE, Physics )

const float PhysicsWorld::GRAVITY_EARTH = -9.80665;

PhysicsWorld::PhysicsWorld( const PhysicsWorldConfiguration &physicsWorldConfiguration )
{
    collisionConfiguration = std::make_shared< btDefaultCollisionConfiguration >( );
    collisionDispatcher = std::make_shared< btCollisionDispatcher >( collisionConfiguration.get( ) );
    overlappingPairCache = std::make_shared< btDbvtBroadphase >( );
    sicSolver = std::make_shared< btSequentialImpulseConstraintSolver >( );

    dynamicsWorld = std::make_shared< btDiscreteDynamicsWorld >( collisionDispatcher.get( ), overlappingPairCache.get( ), sicSolver.get( ), collisionConfiguration.get( ) );
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

    for ( uint32_t i = 0; i < dynamicsWorld->getNumCollisionObjects( ); ++i )
    {
        collisionObjects[ i ]->getUserPointer( );
        dynamicsWorld->removeCollisionObject( collisionObjects[ i ] );
    }
}

END_NAMESPACES
