#pragma once

#include "../Core/Common.h"
#include "../ECS.h"

NAMESPACES( ENGINE_NAMESPACE, Physics )

class CollisionShapeInitializer
{
private:
    std::shared_ptr< ECS::CTransform > transform;
    std::shared_ptr< ECS::CCollisionObject > collisionObject;
    std::shared_ptr< ECS::CRigidBody > rigidBody;
    std::shared_ptr< btCollisionShape > shape;
public:
    CollisionShapeInitializer( std::shared_ptr< ECS::CCollisionObject >  collisionObject, std::shared_ptr< ECS::CTransform > transform );
    CollisionShapeInitializer( std::shared_ptr< ECS::CRigidBody > rigidBody, std::shared_ptr< ECS::CTransform > transform );

    void initializeBoxCollisionShape( const glm::vec3& dimensions );
    void initializeSphereCollisionShape( const float& radius );
private:
    void initializeRequestedComponent( );
};

END_NAMESPACES

