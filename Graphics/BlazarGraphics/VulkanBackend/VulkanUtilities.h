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

#include "VulkanContext.h"
#include "../GraphicsException.h"
#include "../IResourceProvider.h"
#include "VulkanCommandExecutor.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct VulkanTextureWrapper
{
    ResourceUsage previousUsage { };

    vk::Sampler sampler { };
    vk::ImageView imageView { };
    vk::Image image;
    vma::Allocation allocation;
    int mipLevels { };
};

class VulkanContext;

class VulkanUtilities
{
private:
    VulkanUtilities( ) = default;
public:
    static void createSampler( const VulkanContext *context,
                               vk::Sampler &sampler,
                               const vk::SamplerCreateInfo &samplerCreateInfo );

    static void allocateImageMemory( const VulkanContext *context,
                                     const vk::Image &image, vk::DeviceMemory &memory,
                                     const vk::MemoryPropertyFlags &properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent );

    static void initStagingBuffer( const VulkanContext *context,
                                   std::pair< vk::Buffer, vma::Allocation > &stagingBuffer,
                                   const void *data,
                                   const uint64_t &size );

    static vk::SampleCountFlagBits maxDeviceMSAASampleCount( const vk::PhysicalDevice &physicalDevice );

    static vk::Format findSupportedDepthFormat( vk::PhysicalDevice physicalDevice );

    static uint32_t getMatchingMemoryType( const VulkanContext *context,
                                           const vk::MemoryPropertyFlags &memoryProperties,
                                           const vk::MemoryRequirements &memoryRequirements );

    static void createImageView( const VulkanContext *context,
                                 vk::ImageView &imageView,
                                 const vk::Image &image,
                                 const vk::Format &format,
                                 const vk::ImageAspectFlags &aspectFlags,
                                 const uint32_t &layerCount = 1 );

    static void prepareImageForUsage( VulkanCommandExecutor *commandExecutor, VulkanTextureWrapper *textureWrapper, const ResourceUsage &usage );
};

END_NAMESPACES