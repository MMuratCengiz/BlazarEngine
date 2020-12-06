#include "../Core/Common.h"
#include "RenderUtilities.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

void RenderUtilities::copyToDeviceMemory( const vk::Device &device, const vk::DeviceMemory &deviceMemory, const void *data, uint64_t size, uint64_t targetOffset ) {
    void *boundMem = device.mapMemory( deviceMemory, targetOffset, size );
    std::memcpy( boundMem, data, size );
    device.unmapMemory( deviceMemory );
}

void RenderUtilities::createSampler( const std::shared_ptr< InstanceContext > &context, vk::Sampler &sampler, const vk::SamplerCreateInfo &samplerCreateInfo ) {
    sampler = context->logicalDevice.createSampler( samplerCreateInfo );
}

void RenderUtilities::allocateImageMemory( const std::shared_ptr< InstanceContext > &context, const vk::Image &image, vk::DeviceMemory &memory, vk::MemoryRequirements &memoryRequirements, const vk::MemoryPropertyFlags& properties ) {
    memoryRequirements = context->logicalDevice.getImageMemoryRequirements( image );

    vk::MemoryAllocateInfo memoryAllocateInfo { };

    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = getMatchingMemoryType( context, properties, memoryRequirements );

    memory = context->logicalDevice.allocateMemory( memoryAllocateInfo );
}

void RenderUtilities::initStagingBuffer( const std::shared_ptr< InstanceContext > &context, std::pair< vk::Buffer, vma::Allocation > &stagingBuffer, const void *data, const uint64_t &size ) {
    vk::BufferCreateInfo stagingBufferCreateInfo{ };

    stagingBufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
    stagingBufferCreateInfo.size = size;
    stagingBufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo stagingAllocationInfo{ };
    stagingAllocationInfo.usage = vma::MemoryUsage::eCpuToGpu;
    stagingAllocationInfo.requiredFlags = vk::MemoryPropertyFlagBits::eHostVisible;
    stagingAllocationInfo.preferredFlags = vk::MemoryPropertyFlagBits::eHostCoherent;

    stagingBuffer = context->vma.createBuffer( stagingBufferCreateInfo, stagingAllocationInfo );

    void * deviceMemory = context->vma.mapMemory( stagingBuffer.second );
    memcpy( deviceMemory, data, size );
    context->vma.unmapMemory( stagingBuffer.second );
}

vk::SampleCountFlagBits RenderUtilities::maxDeviceMSAASampleCount( const vk::PhysicalDevice &physicalDevice ) {
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

vk::Format RenderUtilities::findSupportedDepthFormat( vk::PhysicalDevice physicalDevice ) {
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

uint32_t RenderUtilities::getMatchingMemoryType( const std::shared_ptr< InstanceContext > &context, const vk::MemoryPropertyFlags &memoryProperties, const vk::MemoryRequirements &memoryRequirements ) {
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
        throw GraphicsException( GraphicsException::Source::Renderer, "Couldn't find matching memory type!" );
    }

    return index;
}

END_NAMESPACES
