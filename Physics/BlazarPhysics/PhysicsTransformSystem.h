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

#pragma once

#include <BlazarCore/Common.h>
#include "PhysicsWorld.h"

NAMESPACES( ENGINE_NAMESPACE, Physics )

class PhysicsTransformSystem
{
private:
    PhysicsWorld* physicsWorld;
public:
    explicit PhysicsTransformSystem( PhysicsWorld* physicsWorld );
    // Care, bypasses physics system, ideally only used in initialization
    static void setPositionRecursive( ECS::IGameEntity *entity, const glm::vec3 &position );
    // Care, bypasses physics system, ideally only used in initialization
    static void setRotationRecursive( ECS::IGameEntity *entity, const ECS::Rotation &rotation );
    // Care, bypasses physics system, ideally only used in initialization
    static void setScaleRecursive( ECS::IGameEntity *entity, const glm::vec3 &scale );

    static void addInstanceRecursive( ECS::IGameEntity *entity, std::shared_ptr< ECS::CTransform > transform );

    void translate( const std::shared_ptr< ECS::IGameEntity > &entity, const glm::vec3 &translation );
    void rotate( const std::shared_ptr< ECS::IGameEntity > &entity, glm::vec3 rotation );
private:
    static btTransform toBtTransform( const std::shared_ptr< ECS::CTransform >& transform );
};


END_NAMESPACES
