#pragma once

#include "../core/Common.h"
#include "GraphicsException.h"

NAMESPACES( SomeVulkan, Graphics )

class RenderUtilities {
private:
    RenderUtilities( ) = default;
public:
    inline static void copyToDeviceMemory(
            const vk::Device &device,
            const vk::DeviceMemory &deviceMemory,
            const void *data,
            vk::DeviceSize size,
            vk::DeviceSize targetOffset = 0 ) {

        void *boundMem = device.mapMemory( deviceMemory, targetOffset, size );
        std::memcpy( boundMem, data, size );
        device.unmapMemory( deviceMemory );
    }

    inline static void createSampler( const pRenderContext &context, vk::Sampler &sampler,
                                      const vk::SamplerCreateInfo &samplerCreateInfo ) {
        sampler = context->logicalDevice.createSampler( samplerCreateInfo );
    }

    inline static void allocateImageMemory( const pRenderContext &context, const vk::Image &image,
                                            vk::DeviceMemory &memory, vk::MemoryRequirements &memoryRequirements,
                                            vk::MemoryPropertyFlags properties =
                                            vk::MemoryPropertyFlagBits::eHostVisible |
                                            vk::MemoryPropertyFlagBits::eHostCoherent ) {
        memoryRequirements = context->logicalDevice.getImageMemoryRequirements( image );

        vk::MemoryAllocateInfo memoryAllocateInfo { };

        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = getMatchingMemoryType( context, properties, memoryRequirements );

        memory = context->logicalDevice.allocateMemory( memoryAllocateInfo );
    }

    inline static void createBufferAndMemory( const pRenderContext &context,
                                              const vk::BufferUsageFlags &usage,
                                              const vk::DeviceSize &requiredSize,
                                              vk::Buffer &buffer,
                                              vk::DeviceMemory &memory,
                                              vk::MemoryPropertyFlags properties ) {

        vk::BufferCreateInfo bufferCreateInfo { };

        bufferCreateInfo.size = requiredSize;
        bufferCreateInfo.usage = usage;
        bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

        buffer = context->logicalDevice.createBuffer( bufferCreateInfo );

        vk::MemoryRequirements memoryRequirements = context->logicalDevice.getBufferMemoryRequirements( buffer );

        vk::MemoryAllocateInfo memoryAllocateInfo { };

        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = getMatchingMemoryType( context, properties, memoryRequirements );

        memory = context->logicalDevice.allocateMemory( memoryAllocateInfo );

        context->logicalDevice.bindBufferMemory( buffer, memory, 0 );
    }

    inline static vk::SampleCountFlagBits maxDeviceMSAASampleCount( const vk::PhysicalDevice &physicalDevice ) {
        vk::PhysicalDeviceProperties properties = physicalDevice.getProperties( );

        vk::SampleCountFlags samples = properties.limits.framebufferColorSampleCounts &
                                       properties.limits.framebufferDepthSampleCounts;

        if ( samples & vk::SampleCountFlagBits::e64 ) { return vk::SampleCountFlagBits::e64; }
        if ( samples & vk::SampleCountFlagBits::e32 ) { return vk::SampleCountFlagBits::e32; }
        if ( samples & vk::SampleCountFlagBits::e16 ) { return vk::SampleCountFlagBits::e16; }
        if ( samples & vk::SampleCountFlagBits::e8 ) { return vk::SampleCountFlagBits::e8; }
        if ( samples & vk::SampleCountFlagBits::e4 ) { return vk::SampleCountFlagBits::e4; }
        if ( samples & vk::SampleCountFlagBits::e2 ) { return vk::SampleCountFlagBits::e2; }

        return vk::SampleCountFlagBits::e1;
    }

    inline static vk::Format findSupportedDepthFormat( vk::PhysicalDevice physicalDevice ) {
        vk::Format desiredFormats[] = { vk::Format::eD24UnormS8Uint, vk::Format::eD32SfloatS8Uint, vk::Format::eD32Sfloat };

        for ( auto format: desiredFormats ) {
            vk::FormatProperties properties = physicalDevice.getFormatProperties( format );

            if ( ( properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment )
                 == vk::FormatFeatureFlagBits::eDepthStencilAttachment ) {
                return format;
            }
        }

        return vk::Format::eD32Sfloat;
    }


    inline static uint32_t getMatchingMemoryType( const pRenderContext &context,
                                                  const vk::MemoryPropertyFlags &memoryProperties,
                                                  const vk::MemoryRequirements &memoryRequirements ) {
        vk::PhysicalDeviceMemoryProperties physicalMemoryProperties = context->physicalDevice.getMemoryProperties( );

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