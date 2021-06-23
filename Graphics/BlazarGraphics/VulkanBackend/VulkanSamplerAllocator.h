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
#include "VulkanUtilities.h"
#include "../AssetManager.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct ShaderInputMaterial
{
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;

    float shininess;
};

struct TextureLoadArguments
{
    VulkanContext* context;
    std::shared_ptr< VulkanCommandExecutor > commandExecutor;

    std::shared_ptr< SamplerDataAttachment > image;
};

class VulkanSamplerAllocator
{
private:
    VulkanSamplerAllocator( ) = default;
public:
    static void load( const TextureLoadArguments &arguments, VulkanTextureWrapper * target );

    static vk::SamplerCreateInfo texToSamplerCreateInfo( const uint32_t &mipLevels, const ECS::Material::TextureInfo &info );
    static vk::Filter toVkFilter( const ECS::Material::Filter &filter );
    static vk::SamplerAddressMode toVkAddressMode( const ECS::Material::AddressMode &filter );
    static vk::SamplerMipmapMode toVkMipmapMode( const ECS::Material::MipmapMode &filter );
private:
    static void generateMipMaps( const TextureLoadArguments &arguments, VulkanTextureWrapper * target, int mipLevels, int width, int height );
};

END_NAMESPACES
