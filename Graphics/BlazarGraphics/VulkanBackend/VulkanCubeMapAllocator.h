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

#include <BlazarECS/ECS.h>
#include "VulkanContext.h"
#include "VulkanCommandExecutor.h"
#include "VulkanSamplerAllocator.h"
#include "VulkanUtilities.h"
#include "../AssetManager.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct CubeMapLoadArguments
{
    const VulkanContext* vulkanContext;
    VulkanCommandExecutor * commandExecutor;

    CubeMapDataAttachment * image;

    explicit CubeMapLoadArguments( const VulkanContext* context ) : vulkanContext( context ) { }
};

class VulkanCubeMapAllocator
{
public:
    static void load( const CubeMapLoadArguments &arguments, VulkanTextureWrapper * target );
};

END_NAMESPACES