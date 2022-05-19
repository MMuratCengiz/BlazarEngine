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

#include <BlazarCore/Utilities.h>
#include "BlazarECS/ECS.h"

NAMESPACES( ENGINE_NAMESPACE, Physics )

struct PhysicsWorldConfiguration
{

};

class PhysicsWorld
{
private:
    btDefaultCollisionConfiguration *collisionConfiguration;
    btCollisionDispatcher *collisionDispatcher;
    btDbvtBroadphase *overlappingPairCache;
    btSequentialImpulseConstraintSolver *sicSolver;

    std::unique_ptr< btDiscreteDynamicsWorld > dynamicsWorld;
    std::vector< ECS::CTransform > collisionTransforms;
public:
    static const float GRAVITY_EARTH;

    explicit PhysicsWorld( const PhysicsWorldConfiguration &physicsWorldConfiguration );
    void addOrUpdateEntity( ECS::IGameEntity *entity );
    void update( ECS::IGameEntity *entity  );
    void tick( );

    ~PhysicsWorld();
};

END_NAMESPACES