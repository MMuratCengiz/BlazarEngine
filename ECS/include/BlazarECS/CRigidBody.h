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
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

// Initialize using BlazarEngine::Physics::CollisionShapeInitializer unless a more custom use case is required.
struct CRigidBody : public IComponent
{
public:
    std::unique_ptr< btCollisionShape > collisionShape = nullptr;
    std::unique_ptr< btMotionState > motionState = nullptr;

    btScalar mass = 0.0;
    std::unique_ptr< btRigidBody > instance = nullptr;

    BLAZAR_COMPONENT( CRigidBody )
};

END_NAMESPACES