#pragma once

#include "VulkanContext.h"
#include "../GraphicsException.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class VulkanContext;

class VulkanUtilities
{
private:
    VulkanUtilities( ) = default;
public:
    static void createSampler( const VulkanContext * context,
                               vk::Sampler &sampler,
                               const vk::SamplerCreateInfo &samplerCreateInfo );

    static void allocateImageMemory( const VulkanContext * context,
                                     const vk::Image &image, vk::DeviceMemory &memory,
                                     const vk::MemoryPropertyFlags &properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent );

    static void initStagingBuffer( const VulkanContext * context,
                                   std::pair< vk::Buffer, vma::Allocation > &stagingBuffer,
                                   const void *data,
                                   const uint64_t &size );

    static vk::SampleCountFlagBits maxDeviceMSAASampleCount( const vk::PhysicalDevice &physicalDevice );

    static vk::Format findSupportedDepthFormat( vk::PhysicalDevice physicalDevice );

    static uint32_t getMatchingMemoryType( const VulkanContext * context,
                                           const vk::MemoryPropertyFlags &memoryProperties,
                                           const vk::MemoryRequirements &memoryRequirements );

    static void createImageView( const VulkanContext * context,
                                 vk::ImageView &imageView,
                                 const vk::Image &image,
                                 const vk::Format &format,
                                 const vk::ImageAspectFlags &aspectFlags,
                                 const uint32_t& layerCount = 1 );
};

END_NAMESPACES