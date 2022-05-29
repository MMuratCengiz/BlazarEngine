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

#include <BlazarGraphics/VulkanBackend/VulkanUtilities.h>
#include <BlazarGraphics/VulkanBackend/CommandExecutorArguments.h>
#include <BlazarGraphics/VulkanBackend/DescriptorManager.h>

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

void VulkanUtilities::prepareImageForUsage( VulkanCommandExecutor * commandExecutor, VulkanTextureWrapper * pWrapper, const ResourceUsage & usage )
{
    FUNCTION_BREAK( usage == pWrapper->previousUsage )

    PipelineBarrierArgs args { };

    args.mipLevel = pWrapper->mipLevels;
    args.image = pWrapper->image;

    auto attachLayoutOld = [ ]( PipelineBarrierArgs &args, const vk::ImageLayout &layout, const vk::AccessFlags &accessFlags, const vk::PipelineStageFlags &stageFlags )
    {
        args.oldLayout = layout;
        args.sourceAccess = accessFlags;
        args.sourceStage = stageFlags;
    };

    auto attachLayoutNew = [ ]( PipelineBarrierArgs &args, const vk::ImageLayout &layout, const vk::AccessFlags &accessFlags, const vk::PipelineStageFlags &stageFlags )
    {
        args.newLayout = layout;
        args.destinationAccess = accessFlags;
        args.destinationStage = stageFlags;
    };

    if ( pWrapper->previousUsage == ResourceUsage::None )
    {
        attachLayoutOld( args, vk::ImageLayout::eUndefined, { }, vk::PipelineStageFlagBits::eTopOfPipe );
    }
    else if ( pWrapper->previousUsage == ResourceUsage::ShaderInputSampler2D )
    {
        attachLayoutOld( args, vk::ImageLayout::eShaderReadOnlyOptimal, vk::AccessFlagBits::eShaderRead, vk::PipelineStageFlagBits::eFragmentShader );
    }
    else if ( pWrapper->previousUsage == ResourceUsage::RenderTarget )
    {
        attachLayoutOld( args, vk::ImageLayout::eColorAttachmentOptimal, vk::AccessFlagBits::eColorAttachmentWrite, vk::PipelineStageFlagBits::eColorAttachmentOutput );
    }

    if ( usage == ResourceUsage::ShaderInputSampler2D )
    {
        attachLayoutNew( args, vk::ImageLayout::eShaderReadOnlyOptimal, vk::AccessFlagBits::eShaderRead, vk::PipelineStageFlagBits::eFragmentShader );
    }
    else if ( usage == ResourceUsage::RenderTarget )
    {
        attachLayoutNew( args, vk::ImageLayout::eColorAttachmentOptimal, vk::AccessFlagBits::eColorAttachmentWrite, vk::PipelineStageFlagBits::eColorAttachmentOutput );
    }

    commandExecutor->startCommandExecution( )
            ->generateBuffers( vk::CommandBufferUsageFlagBits::eOneTimeSubmit, 1 )
            ->beginCommand( )
            ->pipelineBarrier( args )
            ->execute( );
}

END_NAMESPACES
