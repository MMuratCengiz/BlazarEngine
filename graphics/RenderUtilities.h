#pragma once

#include "../core/Common.h"
#include "GraphicsException.h"

NAMESPACES( SomeVulkan, Graphics )

class RenderUtilities {
private:
    RenderUtilities( ) = default;
public:
    inline static void copyToDeviceMemory(
            const VkDevice &device,
            const VkDeviceMemory &deviceMemory,
            const void *data,
            VkDeviceSize size,
            uint32_t sourceOffset = 0,
            uint32_t targetOffset = 0 ) {
        void *boundMem;

        vkMapMemory( device, deviceMemory, sourceOffset, size, targetOffset, &boundMem );
        std::memcpy( boundMem, data, size );
        vkUnmapMemory( device, deviceMemory );
    }

    inline static void createSampler( const pRenderContext &context, VkSampler &sampler,
                                      const VkSamplerCreateInfo &samplerCreateInfo ) {
        if ( vkCreateSampler( context->logicalDevice, &samplerCreateInfo, nullptr, &sampler ) ) {
            throw GraphicsException( GraphicsException::Source::Utilities, "Error creating sampler!" );
        }
    }

    inline static void allocateImageMemory( const pRenderContext &context, const VkImage &image,
                                            VkDeviceMemory &memory, VkMemoryRequirements& memoryRequirements ) {
        vkGetImageMemoryRequirements( context->logicalDevice, image, &memoryRequirements );

        VkMemoryPropertyFlags properties =
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        VkMemoryAllocateInfo memoryAllocateInfo { };

        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = getMatchingMemoryType( context, properties, memoryRequirements );

        if ( vkAllocateMemory( context->logicalDevice, &memoryAllocateInfo, nullptr, &memory ) != VK_SUCCESS ) {
            throw std::runtime_error( "failed to allocate buffer memory!" );
        }
    }

    inline static uint32_t getMatchingMemoryType( const pRenderContext &context,
                                                  const VkMemoryPropertyFlags &memoryProperties,
                                                  const VkMemoryRequirements &memoryRequirements ) {
        VkPhysicalDeviceMemoryProperties physicalMemoryProperties { };
        vkGetPhysicalDeviceMemoryProperties( context->physicalDevice, &physicalMemoryProperties );

        uint32_t typeBits = memoryRequirements.memoryTypeBits;
        uint32_t index = 0;
        bool memTypeFound = false;

        for ( ; index < physicalMemoryProperties.memoryTypeCount; index++ ) {
            if ( ( typeBits & 1 ) == 1 ) {
                if ( ( physicalMemoryProperties.memoryTypes[ index ].propertyFlags & memoryProperties ) ==
                     memoryProperties ) {
                    memTypeFound = true;
                    break;
                }
            }

            typeBits >>= 1;
        }

        if ( !memTypeFound ) {
            throw SomeVulkan::Graphics::GraphicsException( SomeVulkan::Graphics::GraphicsException::Source::Renderer,
                                                           "Couldn't find matching memory type!" );
        }

        return index;
    }
};

END_NAMESPACES