#include "VulkanUtilities.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

void VulkanUtilities::createSampler( const VulkanContext * context, vk::Sampler &sampler, const vk::SamplerCreateInfo &samplerCreateInfo )
{
    sampler = context->logicalDevice.createSampler( samplerCreateInfo );
}

void VulkanUtilities::allocateImageMemory( const VulkanContext * context, const vk::Image &image, vk::DeviceMemory &memory, const vk::MemoryPropertyFlags &properties )
{
    vk::MemoryRequirements memoryRequirements = context->logicalDevice.getImageMemoryRequirements( image );

    vk::MemoryAllocateInfo memoryAllocateInfo { };

    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = getMatchingMemoryType( context, properties, memoryRequirements );

    memory = context->logicalDevice.allocateMemory( memoryAllocateInfo );
}

void VulkanUtilities::initStagingBuffer( const VulkanContext * context, std::pair< vk::Buffer, vma::Allocation > &stagingBuffer, const void *data, const uint64_t &size )
{
    vk::BufferCreateInfo stagingBufferCreateInfo { };

    stagingBufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
    stagingBufferCreateInfo.size = size;
    stagingBufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo stagingAllocationInfo { };
    stagingAllocationInfo.usage = vma::MemoryUsage::eCpuToGpu;
    stagingAllocationInfo.requiredFlags = vk::MemoryPropertyFlagBits::eHostVisible;
    stagingAllocationInfo.preferredFlags = vk::MemoryPropertyFlagBits::eHostCoherent;

    stagingBuffer = context->vma.createBuffer( stagingBufferCreateInfo, stagingAllocationInfo );

    void *deviceMemory = context->vma.mapMemory( stagingBuffer.second );
    memcpy( deviceMemory, data, size );
    context->vma.unmapMemory( stagingBuffer.second );
}

vk::SampleCountFlagBits VulkanUtilities::maxDeviceMSAASampleCount( const vk::PhysicalDevice &physicalDevice )
{
    vk::PhysicalDeviceProperties properties = physicalDevice.getProperties( );

    vk::SampleCountFlags samples = properties.limits.framebufferColorSampleCounts &
                                   properties.limits.framebufferDepthSampleCounts;

    if ( samples & vk::SampleCountFlagBits::e64 )
    { return vk::SampleCountFlagBits::e64; }
    if ( samples & vk::SampleCountFlagBits::e32 )
    { return vk::SampleCountFlagBits::e32; }
    if ( samples & vk::SampleCountFlagBits::e16 )
    { return vk::SampleCountFlagBits::e16; }
    if ( samples & vk::SampleCountFlagBits::e8 )
    { return vk::SampleCountFlagBits::e8; }
    if ( samples & vk::SampleCountFlagBits::e4 )
    { return vk::SampleCountFlagBits::e4; }
    if ( samples & vk::SampleCountFlagBits::e2 )
    { return vk::SampleCountFlagBits::e2; }

    return vk::SampleCountFlagBits::e1;
}

vk::Format VulkanUtilities::findSupportedDepthFormat( vk::PhysicalDevice physicalDevice )
{
    vk::Format desiredFormats[] = { vk::Format::eD24UnormS8Uint, vk::Format::eD32SfloatS8Uint, vk::Format::eD32Sfloat };

    for ( auto format: desiredFormats )
    {
        vk::FormatProperties properties = physicalDevice.getFormatProperties( format );

        if ( ( properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment )
             == vk::FormatFeatureFlagBits::eDepthStencilAttachment )
        {
            return format;
        }
    }

    return vk::Format::eD32Sfloat;
}

uint32_t VulkanUtilities::getMatchingMemoryType( const VulkanContext * context, const vk::MemoryPropertyFlags &memoryProperties, const vk::MemoryRequirements &memoryRequirements )
{
    vk::PhysicalDeviceMemoryProperties physicalMemoryProperties = context->physicalDevice.getMemoryProperties( );

    uint32_t typeBits = memoryRequirements.memoryTypeBits;
    uint32_t index = 0;
    bool memTypeFound = false;

    for ( ; index < physicalMemoryProperties.memoryTypeCount; index++ )
    {
        if ( ( typeBits & 1 ) == 1 )
        {
            if ( ( physicalMemoryProperties.memoryTypes[ index ].propertyFlags & memoryProperties ) ==
                 memoryProperties )
            {
                memTypeFound = true;
                break;
            }
        }

        typeBits >>= 1;
    }

    if ( !memTypeFound )
    {
        throw GraphicsException( "Renderer", "Couldn't find matching memory type!" );
    }

    return index;
}

void VulkanUtilities::createImageView( const VulkanContext * context,
                                       vk::ImageView &imageView,
                                       const vk::Image &image,
                                       const vk::Format &format,
                                       const vk::ImageAspectFlags &aspectFlags,
                                       const uint32_t &layerCount )
{
    vk::ImageViewCreateInfo imageViewCreateInfo { };
    imageViewCreateInfo.image = image;
    imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
    imageViewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
    imageViewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
    imageViewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;
    imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = layerCount;

    imageView = context->logicalDevice.createImageView( imageViewCreateInfo );
}

END_NAMESPACES
