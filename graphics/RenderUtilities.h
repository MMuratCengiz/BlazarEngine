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

        VkResult mapResult = vkMapMemory( device, deviceMemory, sourceOffset, size, targetOffset, &boundMem );

        if ( mapResult == VK_SUCCESS ) {
            std::memcpy( boundMem, data, size );
            vkUnmapMemory( device, deviceMemory );
        }
    }

    inline static void createSampler( const pRenderContext &context, VkSampler &sampler,
                                      const VkSamplerCreateInfo &samplerCreateInfo ) {
        if ( vkCreateSampler( context->logicalDevice, &samplerCreateInfo, nullptr, &sampler ) ) {
            throw GraphicsException( GraphicsException::Source::Utilities, "Error creating sampler!" );
        }
    }

    inline static void allocateImageMemory( const pRenderContext &context, const VkImage &image,
                                            VkDeviceMemory &memory, VkMemoryRequirements &memoryRequirements,
                                            VkMemoryPropertyFlags properties =
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ) {
        vkGetImageMemoryRequirements( context->logicalDevice, image, &memoryRequirements );

        VkMemoryAllocateInfo memoryAllocateInfo { };

        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = getMatchingMemoryType( context, properties, memoryRequirements );

        if ( vkAllocateMemory( context->logicalDevice, &memoryAllocateInfo, nullptr, &memory ) != VK_SUCCESS ) {
            throw std::runtime_error( "failed to allocate buffer memory!" );
        }
    }

    inline static void createBufferAndMemory( const pRenderContext &context,
                                              const VkBufferUsageFlags& usage,
                                              const VkDeviceSize& requiredSize,
                                              VkBuffer &buffer,
                                              VkDeviceMemory &memory,
                                              VkMemoryPropertyFlags properties ) {

        VkBufferCreateInfo bufferCreateInfo { };

        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = requiredSize;
        bufferCreateInfo.usage = usage;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if ( vkCreateBuffer( context->logicalDevice, &bufferCreateInfo, nullptr, &buffer ) != VK_SUCCESS ) {
            throw std::runtime_error( "failed to create vertex buffer!" );
        }

        VkMemoryRequirements memoryRequirements{ };

        vkGetBufferMemoryRequirements( context->logicalDevice, buffer, &memoryRequirements );

        VkMemoryAllocateInfo memoryAllocateInfo { };

        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = getMatchingMemoryType( context, properties, memoryRequirements );

        if ( vkAllocateMemory( context->logicalDevice, &memoryAllocateInfo, nullptr, &memory ) != VK_SUCCESS ) {
            throw std::runtime_error( "failed to allocate buffer memory!" );
        }

        vkBindBufferMemory( context->logicalDevice, buffer, memory, 0 );
    }

    inline static VkSampleCountFlagBits maxDeviceMSAASampleCount( const VkPhysicalDevice& physicalDevice ) {
        VkPhysicalDeviceProperties properties;

        vkGetPhysicalDeviceProperties( physicalDevice, &properties );

        VkSampleCountFlags samples = properties.limits.framebufferColorSampleCounts &
                                     properties.limits.framebufferDepthSampleCounts;

        if ( samples & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
        if ( samples & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
        if ( samples & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
        if ( samples & VK_SAMPLE_COUNT_8_BIT ) { return VK_SAMPLE_COUNT_8_BIT;  }
        if ( samples & VK_SAMPLE_COUNT_4_BIT ) { return VK_SAMPLE_COUNT_4_BIT;  }
        if ( samples & VK_SAMPLE_COUNT_2_BIT ) { return VK_SAMPLE_COUNT_2_BIT;  }

        return VK_SAMPLE_COUNT_1_BIT;
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