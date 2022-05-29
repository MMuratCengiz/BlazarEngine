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

#include <BlazarGraphics/VulkanBackend/VulkanCubeMapAllocator.h>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

void VulkanCubeMapAllocator::load( const CubeMapLoadArguments &arguments, VulkanTextureWrapper *target )
{
    const auto &context = arguments.vulkanContext;
    const auto &commandExecutor = arguments.commandExecutor;
    const auto &image = arguments.image;

    ASSERT_M( !image->images.empty( ), "Cube map textures cannot be empty!" );

    int width = image->images[ 0 ]->width;
    int height = image->images[ 0 ]->height;

    std::vector< std::pair< vk::Buffer, vma::Allocation > > stagingBuffers( arguments.image->images.size( ) );

    int mipStagingBufferIndex = 0;

    for ( const auto &img: arguments.image->images )
    {
        std::pair< vk::Buffer, vma::Allocation > &stagingBuffer = stagingBuffers[ mipStagingBufferIndex++ ];

        VulkanUtilities::initStagingBuffer( context, stagingBuffer, img->content, img->width * img->height * 4 );
    }

    vk::ImageCreateInfo imageCreateInfo { };

    imageCreateInfo.imageType = vk::ImageType::e2D;
    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 6;
    imageCreateInfo.format = vk::Format::eR8G8B8A8Srgb;
    imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
    imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
    imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
    imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
    imageCreateInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;

    vma::AllocationCreateInfo allocationCreateInfo { };
    allocationCreateInfo.usage = vma::MemoryUsage::eGpuOnly;

    auto imageAllocationPair = context->vma.createImage( imageCreateInfo, allocationCreateInfo );
    target->image = imageAllocationPair.first;
    target->allocation = imageAllocationPair.second;

    vk::ImageViewCreateInfo imageViewCreateInfo { };

    imageViewCreateInfo.image = target->image;
    imageViewCreateInfo.viewType = vk::ImageViewType::eCube;
    imageViewCreateInfo.format = vk::Format::eR8G8B8A8Srgb;
    imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 6;

    target->imageView = context->logicalDevice.createImageView( imageViewCreateInfo );

    ECS::Material::TextureInfo cubeMapTextureInfo { };
    cubeMapTextureInfo.minFilter = ECS::Material::Filter::Linear;
    cubeMapTextureInfo.magFilter = ECS::Material::Filter::Linear;
    cubeMapTextureInfo.mipmapMode = ECS::Material::MipmapMode::eLinear;
    cubeMapTextureInfo.U = ECS::Material::AddressMode::ClampToEdge;
    cubeMapTextureInfo.V = ECS::Material::AddressMode::ClampToEdge;
    cubeMapTextureInfo.W = ECS::Material::AddressMode::ClampToEdge;
    cubeMapTextureInfo.minLod = 1.0f;
    cubeMapTextureInfo.maxLod = 1.0f;
    cubeMapTextureInfo.mipLodBias = 0.0f;

    target->sampler = context->logicalDevice.createSampler( VulkanSamplerAllocator::texToSamplerCreateInfo( 1, cubeMapTextureInfo ) );


    int arrayLayer = 0;
    for ( auto &stagingBuffer: stagingBuffers )
    {
        PipelineBarrierArgs toTransferOptimal { };

        toTransferOptimal.mipLevel = 1;
        toTransferOptimal.baseArrayLayer = arrayLayer;
        toTransferOptimal.layerCount = 1;
        toTransferOptimal.image = target->image;
        toTransferOptimal.oldLayout = vk::ImageLayout::eUndefined;
        toTransferOptimal.newLayout = vk::ImageLayout::eTransferDstOptimal;
        toTransferOptimal.sourceAccess = { };
        toTransferOptimal.destinationAccess = vk::AccessFlagBits::eTransferWrite;
        toTransferOptimal.sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        toTransferOptimal.destinationStage = vk::PipelineStageFlagBits::eTransfer;

        CopyBufferToImageArgs copyBufferToImageArgs { };
        copyBufferToImageArgs.image = target->image;
        copyBufferToImageArgs.sourceBuffer = stagingBuffer.first;
        copyBufferToImageArgs.width = width;
        copyBufferToImageArgs.height = height;
        copyBufferToImageArgs.arrayLayer = arrayLayer;
        copyBufferToImageArgs.mipLevel = 0;

        PipelineBarrierArgs toShaderOptimal { };

        toShaderOptimal.mipLevel = 1;
        toShaderOptimal.baseArrayLayer = arrayLayer;
        toShaderOptimal.layerCount = 1;
        toShaderOptimal.image = target->image;
        toShaderOptimal.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        toShaderOptimal.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        toShaderOptimal.sourceAccess = { };
        toShaderOptimal.destinationAccess = vk::AccessFlagBits::eTransferWrite;
        toShaderOptimal.sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        toShaderOptimal.destinationStage = vk::PipelineStageFlagBits::eTransfer;

        commandExecutor->startCommandExecution( )
                ->generateBuffers( vk::CommandBufferUsageFlagBits::eOneTimeSubmit, 1 )
                ->beginCommand( )
                ->pipelineBarrier( toTransferOptimal )
                ->copyBufferToImage( copyBufferToImageArgs )
                ->pipelineBarrier( toShaderOptimal )
                ->execute( );

        context->vma.destroyBuffer( stagingBuffer.first, stagingBuffer.second );
        arrayLayer++;
    }
}

END_NAMESPACES
