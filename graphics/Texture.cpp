//
// Created by Murat on 11/1/2020.
//

#include "Texture.h"

NAMESPACES( SomeVulkan, Graphics )

TextureLoader::TextureLoader( uint8_t dimension, const std::string &path, TextureInfo textureInfo )
        : dimension( dimension ), path( path ), textureInfo( textureInfo ) {
    int lWidth, lHeight;

    contents = stbi_load( ( PATH( path ) ).c_str( ), &lWidth, &lHeight, &channels,
                          STBI_rgb_alpha );

    if ( contents == nullptr ) {
        TRACE( COMPONENT_TLOAD, VERBOSITY_CRITICAL, stbi_failure_reason( ) )

        throw std::runtime_error( "Couldn't find texture." );
    }

    width = static_cast< uint32_t >( lWidth );
    height = static_cast< uint32_t >( lHeight );
    mipLevels = std::floor( std::log2( std::max( width, height ) ) ) + 1;
}

uint32_t TextureLoader::size( ) const {
    return getWidth( ) * getHeight( ) * 4;
}

void TextureLoader::loadIntoGPUMemory( std::shared_ptr< InstanceContext > &context, pCommandExecutor &commandExecutor ) {
    if ( isLoadedToGPUMemory ) {
        return;
    }

    this->context = context;
    this->device = context->logicalDevice;

    vk::BufferCreateInfo stagingBufferCreateInfo{ };
    stagingBufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
    stagingBufferCreateInfo.size = size( );
    stagingBufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo stagingAllocationInfo{ };
    stagingAllocationInfo.usage = vma::MemoryUsage::eCpuToGpu;
    stagingAllocationInfo.requiredFlags = vk::MemoryPropertyFlagBits::eHostVisible;
    stagingAllocationInfo.preferredFlags = vk::MemoryPropertyFlagBits::eHostCoherent;

    auto stagingBuffer = context->vma.createBuffer( stagingBufferCreateInfo, stagingAllocationInfo );

    void * data = context->vma.mapMemory( stagingBuffer.second );
    memcpy( data, contents, size() );
    context->vma.unmapMemory( stagingBuffer.second );

    vk::ImageCreateInfo imageCreateInfo { };

    imageCreateInfo.imageType = vk::ImageType::e2D;
    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = mipLevels;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = vk::Format::eR8G8B8A8Srgb;
    imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
    imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst
                            | vk::ImageUsageFlagBits::eSampled;
    imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
    imageCreateInfo.samples = vk::SampleCountFlagBits::e1;

    vma::AllocationCreateInfo allocationCreateInfo { };
    allocationCreateInfo.usage = vma::MemoryUsage::eGpuOnly;

    auto imageAllocationPair = context->vma.createImage( imageCreateInfo, allocationCreateInfo );
    image = imageAllocationPair.first;
    imageAllocation = imageAllocationPair.second;

    isLoadedToGPUMemory = true;

    vk::ImageViewCreateInfo imageViewCreateInfo { };

    imageViewCreateInfo.image = image;
    imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
    imageViewCreateInfo.format = vk::Format::eR8G8B8A8Srgb;
    imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = mipLevels;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    imageView = context->logicalDevice.createImageView( imageViewCreateInfo );

    vk::SamplerCreateInfo samplerCreateInfo { };

    samplerCreateInfo.magFilter = textureInfo.magFilter;
    samplerCreateInfo.minFilter = textureInfo.minFilter;
    samplerCreateInfo.addressModeU = textureInfo.addressMode.U;
    samplerCreateInfo.addressModeV = textureInfo.addressMode.V;
    samplerCreateInfo.addressModeW = textureInfo.addressMode.W;
    samplerCreateInfo.anisotropyEnable = VK_TRUE;
    samplerCreateInfo.maxAnisotropy = 16.0f;
    samplerCreateInfo.borderColor = vk::BorderColor::eFloatOpaqueBlack;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.compareOp = vk::CompareOp::eAlways;
    samplerCreateInfo.mipmapMode = textureInfo.mipmapMode;
    samplerCreateInfo.mipLodBias = textureInfo.mipLodBias;
    samplerCreateInfo.minLod = textureInfo.minLod;
    samplerCreateInfo.maxLod = mipLevels;

    sampler = context->logicalDevice.createSampler( samplerCreateInfo );

    PipelineBarrierArgs args { };

    args.mipLevel = mipLevels;
    args.image = image;
    args.oldLayout = vk::ImageLayout::eUndefined;
    args.newLayout = vk::ImageLayout::eTransferDstOptimal;
    args.sourceAccess = { };
    args.destinationAccess = vk::AccessFlagBits::eTransferWrite;
    args.sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
    args.destinationStage = vk::PipelineStageFlagBits::eTransfer;

    CopyBufferToImageArgs copyBufferToImageArgs { };
    copyBufferToImageArgs.image = image;
    copyBufferToImageArgs.sourceBuffer = stagingBuffer.first;
    copyBufferToImageArgs.width = width;
    copyBufferToImageArgs.height = height;

    commandExecutor->startCommandExecution( )
            ->generateBuffers( vk::CommandBufferUsageFlagBits::eOneTimeSubmit, 1 )
            ->beginCommand( )
            ->pipelineBarrier( args )
            ->copyBufferToImage( copyBufferToImageArgs )
            ->execute( );

    context->vma.destroyBuffer( stagingBuffer.first, stagingBuffer.second );

    vk::FormatProperties properties = context->physicalDevice.getFormatProperties( vk::Format::eR8G8B8A8Srgb );

    if ( ( properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear )
         != vk::FormatFeatureFlagBits::eSampledImageFilterLinear ) {
        throw std::runtime_error( "Unsupported device, VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT must be "
                                  "supported" );
    }

    generateMipMaps( context, commandExecutor );
}

void TextureLoader::generateMipMaps( std::shared_ptr< InstanceContext > &context,
                               std::shared_ptr< CommandExecutor > &commandExecutor ) const {
    int32_t mipWidth = width, mipHeight = height;

    auto cmdBuffer = commandExecutor->startCommandExecution( )
            ->generateBuffers( vk::CommandBufferUsageFlagBits::eOneTimeSubmit, 1 );

    cmdBuffer->beginCommand( );

    PipelineBarrierArgs pipelineBarrierArgs { };
    ImageBlitArgs blitArgs { };

    /*
     * Continuously copy current image( in mip level=index ) to the next, ie.
     * Iteration 1: i - 1 = 0, 512x512 is copied to i as 512/2
     * Iteration 2: i - 1 = 0, 512/2 is copied to i as 512/2/2
     *
     * In such form every mip level of the image is filled with an image with half the size of the previous image
     */
    for ( uint32_t index = 1; index < mipLevels; ++index ) {
        /* transition every index - 1 image as the source image */
        pipelineBarrierArgs.baseMipLevel = index - 1;
        pipelineBarrierArgs.mipLevel = 1;
        pipelineBarrierArgs.image = image;
        pipelineBarrierArgs.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        pipelineBarrierArgs.newLayout = vk::ImageLayout::eTransferSrcOptimal;
        pipelineBarrierArgs.sourceAccess = vk::AccessFlagBits::eTransferWrite;
        pipelineBarrierArgs.destinationAccess = vk::AccessFlagBits::eTransferRead;
        pipelineBarrierArgs.sourceStage = vk::PipelineStageFlagBits::eTransfer;
        pipelineBarrierArgs.destinationStage = vk::PipelineStageFlagBits::eTransfer;

        cmdBuffer->pipelineBarrier( pipelineBarrierArgs );


        blitArgs.srcOffsets[ 0 ] = vk::Offset3D { 0, 0, 0 };
        blitArgs.srcOffsets[ 1 ] = vk::Offset3D { mipWidth, mipHeight, 1 };
        blitArgs.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        blitArgs.srcSubresource.mipLevel = index - 1;
        blitArgs.srcSubresource.baseArrayLayer = 0;
        blitArgs.srcSubresource.layerCount = 1;
        blitArgs.dstOffsets[ 0 ] = vk::Offset3D { 0, 0, 0 };
        blitArgs.dstOffsets[ 1 ] = vk::Offset3D { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1,
                                                  1 };
        blitArgs.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        blitArgs.dstSubresource.mipLevel = index;
        blitArgs.dstSubresource.baseArrayLayer = 0;
        blitArgs.dstSubresource.layerCount = 1;
        blitArgs.sourceImage = image;
        blitArgs.destinationImage = image;
        blitArgs.sourceImageLayout = vk::ImageLayout::eTransferSrcOptimal;
        blitArgs.destinationImageLayout = vk::ImageLayout::eTransferDstOptimal;

        cmdBuffer->blitImage( blitArgs );

        /* After the blit transition the image to the form that will be used by the shader */

        pipelineBarrierArgs.baseMipLevel = index - 1;
        pipelineBarrierArgs.mipLevel = 1;
        pipelineBarrierArgs.image = image;
        pipelineBarrierArgs.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
        pipelineBarrierArgs.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        pipelineBarrierArgs.sourceAccess = vk::AccessFlagBits::eTransferRead;
        pipelineBarrierArgs.destinationAccess = vk::AccessFlagBits::eShaderRead;
        pipelineBarrierArgs.sourceStage = vk::PipelineStageFlagBits::eTransfer;
        pipelineBarrierArgs.destinationStage = vk::PipelineStageFlagBits::eFragmentShader;

        cmdBuffer->pipelineBarrier( pipelineBarrierArgs );

        if ( mipWidth > 1 ) { mipWidth /= 2; }
        if ( mipHeight > 1 ) { mipHeight /= 2; }
    }

    pipelineBarrierArgs.baseMipLevel = mipLevels - 1;
    pipelineBarrierArgs.mipLevel = 1;
    pipelineBarrierArgs.image = image;
    pipelineBarrierArgs.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    pipelineBarrierArgs.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    pipelineBarrierArgs.sourceAccess = vk::AccessFlagBits::eTransferRead;
    pipelineBarrierArgs.destinationAccess = vk::AccessFlagBits::eShaderRead;
    pipelineBarrierArgs.sourceStage = vk::PipelineStageFlagBits::eTransfer;
    pipelineBarrierArgs.destinationStage = vk::PipelineStageFlagBits::eFragmentShader;

    cmdBuffer->pipelineBarrier( pipelineBarrierArgs );
    cmdBuffer->execute( );
}

TextureLoader::~TextureLoader( ) = default;

void TextureLoader::unload( ) {
    stbi_image_free( contents );

    if ( isLoadedToGPUMemory ) {
        device.destroySampler( sampler );
        device.destroyImageView( imageView );
        context->vma.destroyImage( image, imageAllocation );
    }
}

END_NAMESPACES