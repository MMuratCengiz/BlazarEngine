#pragma once

#include "../Core/Common.h"
#include "InstanceContext.h"
#include "GraphicsException.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class InstanceContext;

class RenderUtilities
{
private:
    RenderUtilities( ) = default;
public:
    static void copyToDeviceMemory(
            const vk::Device &device,
            const vk::DeviceMemory &deviceMemory,
            const void *data,
            vk::DeviceSize size,
            vk::DeviceSize targetOffset = 0 );

    static void createSampler( const std::shared_ptr< InstanceContext > &context, vk::Sampler &sampler,
                               const vk::SamplerCreateInfo &samplerCreateInfo );

    static void allocateImageMemory( const std::shared_ptr< InstanceContext > &context, const vk::Image &image,
                                     vk::DeviceMemory &memory, vk::MemoryRequirements &memoryRequirements,
                                     const vk::MemoryPropertyFlags &properties =
                                     vk::MemoryPropertyFlagBits::eHostVisible |
                                     vk::MemoryPropertyFlagBits::eHostCoherent );

    static void initStagingBuffer( const std::shared_ptr< InstanceContext > &context, std::pair< vk::Buffer, vma::Allocation > &stagingBuffer, const void *data, const uint64_t &size );

    static vk::SampleCountFlagBits maxDeviceMSAASampleCount( const vk::PhysicalDevice &physicalDevice );

    static vk::Format findSupportedDepthFormat( vk::PhysicalDevice physicalDevice );


    static uint32_t getMatchingMemoryType( const std::shared_ptr< InstanceContext > &context,
                                           const vk::MemoryPropertyFlags &memoryProperties,
                                           const vk::MemoryRequirements &memoryRequirements );
};

END_NAMESPACES