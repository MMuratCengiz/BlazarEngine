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

enum class RotationUnit
{
    Radians,
    Degrees
};

struct Rotation
{
    RotationUnit rotationUnit { RotationUnit::Degrees };
    glm::vec3 euler { 0.0f };
};

struct CTransform : public IComponent
{
public:
    glm::vec3 position { 0.0f };
    glm::vec3 scale { 1.0f };
    Rotation rotation { };

    BLAZAR_COMPONENT( CTransform )
};


END_NAMESPACES