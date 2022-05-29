// Blazar Engine - 3D Game Engine
// Copyright (c) 2020-2021 Muhammed Murat Cengiz
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <BlazarCore/Common.h>
#include <BlazarCore/Utilities.h>
#include <BlazarECS/ECS.h>
#include <btBulletDynamicsCommon.h>

NAMESPACES( ENGINE_NAMESPACE, Physics )

class CollisionShapeInitializer
{
private:
    ECS::CTransform * transform;
    ECS::CCollisionObject * collisionObject;
    ECS::CRigidBody * rigidBody;
    std::unique_ptr< btCollisionShape > shape;
public:
    CollisionShapeInitializer( ECS::CCollisionObject *  collisionObject, ECS::CTransform * transform );
    CollisionShapeInitializer( ECS::CRigidBody * rigidBody, ECS::CTransform * transform );

    void initializeBoxCollisionShape( const glm::vec3& dimensions );
    void initializeSphereCollisionShape( const float& radius );
private:
    void initializeRequestedComponent( );
};

END_NAMESPACES

