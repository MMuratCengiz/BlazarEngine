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

#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

struct CCamera : public IComponent
{
public:
    bool isActive = true;
    glm::mat4 view { };
    glm::mat4 projection { };
    glm::vec3 position { };

    BLAZAR_COMPONENT( CCamera )
};

END_NAMESPACES