//
// Created by Murat on 11/1/2020.
//

#include "Texture.h"
#include "RenderUtilities.h"

NAMESPACES( SomeVulkan, Graphics )

Texture::Texture( uint8_t dimension, const std::string &path, TextureInfo textureInfo )
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

uint32_t Texture::size( ) const {
    return getWidth( ) * getHeight( ) * 4;
}

void Texture::loadIntoGPUMemory( std::shared_ptr< RenderContext > &context, pCommandExecutor &commandExecutor ) {
    if ( isLoadedToGPUMemory ) {
        return;
    }

    this->device = context->logicalDevice;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    RenderUtilities::createBufferAndMemory(
            context,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            size( ),
            stagingBuffer,
            stagingBufferMemory,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );


    RenderUtilities::copyToDeviceMemory( context->logicalDevice, stagingBufferMemory, contents, size( ) );

    VkImageCreateInfo imageCreateInfo { };

    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = mipLevels;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage =
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    if ( vkCreateImage( context->logicalDevice, &imageCreateInfo, nullptr, &textureGPUBuffer.buffer.image ) !=
         VK_SUCCESS ) {
        throw GraphicsException( GraphicsException::Source::Renderer, "Failed to create image!" );
    }

    VkMemoryRequirements memoryRequirements { };

    RenderUtilities::allocateImageMemory( context, textureGPUBuffer.buffer.image, textureGPUBuffer.memory,
                                          memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

    vkBindImageMemory( context->logicalDevice, textureGPUBuffer.buffer.image, textureGPUBuffer.memory, 0 );

    isLoadedToGPUMemory = true;

    VkImageViewCreateInfo imageViewCreateInfo { };

    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = textureGPUBuffer.buffer.image;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = mipLevels;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    if ( vkCreateImageView( context->logicalDevice, &imageViewCreateInfo, nullptr, &imageView ) != VK_SUCCESS ) {
        throw GraphicsException { GraphicsException::Source::RenderSurface, "Could not create image view!" };
    }

    VkSamplerCreateInfo samplerCreateInfo { };

    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    samplerCreateInfo.magFilter = textureInfo.magFilter;
    samplerCreateInfo.minFilter = textureInfo.minFilter;
    samplerCreateInfo.addressModeU = textureInfo.addressMode.U;
    samplerCreateInfo.addressModeV = textureInfo.addressMode.V;
    samplerCreateInfo.addressModeW = textureInfo.addressMode.W;
    samplerCreateInfo.anisotropyEnable = VK_TRUE;
    samplerCreateInfo.maxAnisotropy = 16.0f;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerCreateInfo.mipmapMode = textureInfo.mipmapMode;
    samplerCreateInfo.mipLodBias = textureInfo.mipLodBias;
    samplerCreateInfo.minLod = textureInfo.minLod;
    samplerCreateInfo.maxLod = mipLevels;

    if ( vkCreateSampler( context->logicalDevice, &samplerCreateInfo, nullptr, &sampler ) != VK_SUCCESS ) {
        throw GraphicsException { GraphicsException::Source::RenderSurface, "Could not create image view!" };
    }

    PipelineBarrierArgs args { };

    args.mipLevel = mipLevels;
    args.image = textureGPUBuffer.buffer.image;
    args.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    args.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    args.sourceAccess = 0;
    args.destinationAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
    args.sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    args.destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

    CopyBufferToImageArgs copyBufferToImageArgs { };
    copyBufferToImageArgs.image = textureGPUBuffer.buffer.image;
    copyBufferToImageArgs.sourceBuffer = stagingBuffer;
    copyBufferToImageArgs.width = width;
    copyBufferToImageArgs.height = height;

    commandExecutor->startCommandExecution( )
            ->generateBuffers( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, 1 )
            ->beginCommand( )
            ->pipelineBarrier( args )
            ->copyBufferToImage( copyBufferToImageArgs )
            ->execute( );

    vkDestroyBuffer( context->logicalDevice, stagingBuffer, nullptr );
    vkFreeMemory( context->logicalDevice, stagingBufferMemory, nullptr );

    VkFormatProperties properties;
    vkGetPhysicalDeviceFormatProperties( context->physicalDevice, VK_FORMAT_R8G8B8A8_SRGB, &properties );

    if ( ( properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT ) == 0 ) {
        throw std::runtime_error( "Unsupported device, VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT must be "
                                  "supported" );
    }

    generateMipMaps( context, commandExecutor );

}

void Texture::generateMipMaps( std::shared_ptr< RenderContext > &context,
                               std::shared_ptr< CommandExecutor > &commandExecutor ) const {
    int32_t mipWidth = width, mipHeight = height;

    auto cmdBuffer = commandExecutor->startCommandExecution( )
            ->generateBuffers( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, 1 );

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
        pipelineBarrierArgs.image = textureGPUBuffer.buffer.image;
        pipelineBarrierArgs.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        pipelineBarrierArgs.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        pipelineBarrierArgs.sourceAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
        pipelineBarrierArgs.destinationAccess = VK_ACCESS_TRANSFER_READ_BIT;
        pipelineBarrierArgs.sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        pipelineBarrierArgs.destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

        cmdBuffer->pipelineBarrier( pipelineBarrierArgs );


        blitArgs.srcOffsets[ 0 ] = { 0, 0, 0 };
        blitArgs.srcOffsets[ 1 ] = { mipWidth, mipHeight, 1 };
        blitArgs.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blitArgs.srcSubresource.mipLevel = index - 1;
        blitArgs.srcSubresource.baseArrayLayer = 0;
        blitArgs.srcSubresource.layerCount = 1;
        blitArgs.dstOffsets[ 0 ] = { 0, 0, 0 };
        blitArgs.dstOffsets[ 1 ] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blitArgs.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blitArgs.dstSubresource.mipLevel = index;
        blitArgs.dstSubresource.baseArrayLayer = 0;
        blitArgs.dstSubresource.layerCount = 1;
        blitArgs.sourceImage = textureGPUBuffer.buffer.image;
        blitArgs.destinationImage = textureGPUBuffer.buffer.image;
        blitArgs.sourceImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        blitArgs.destinationImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        cmdBuffer->blitImage( blitArgs );

        /* After the blit transition the image to the form that will be used by the shader */

        pipelineBarrierArgs.baseMipLevel = index - 1;
        pipelineBarrierArgs.mipLevel = 1;
        pipelineBarrierArgs.image = textureGPUBuffer.buffer.image;
        pipelineBarrierArgs.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        pipelineBarrierArgs.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        pipelineBarrierArgs.sourceAccess = VK_ACCESS_TRANSFER_READ_BIT;
        pipelineBarrierArgs.destinationAccess = VK_ACCESS_SHADER_READ_BIT;
        pipelineBarrierArgs.sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        pipelineBarrierArgs.destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

        cmdBuffer->pipelineBarrier( pipelineBarrierArgs );


        if ( mipWidth > 1 ) { mipWidth /= 2; }
        if ( mipHeight > 1 ) { mipHeight /= 2; }
    }

    pipelineBarrierArgs.baseMipLevel = mipLevels - 1;
    pipelineBarrierArgs.mipLevel = 1;
    pipelineBarrierArgs.image = textureGPUBuffer.buffer.image;
    pipelineBarrierArgs.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    pipelineBarrierArgs.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    pipelineBarrierArgs.sourceAccess = VK_ACCESS_TRANSFER_READ_BIT;
    pipelineBarrierArgs.destinationAccess = VK_ACCESS_SHADER_READ_BIT;
    pipelineBarrierArgs.sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    pipelineBarrierArgs.destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    cmdBuffer->pipelineBarrier( pipelineBarrierArgs );
    cmdBuffer->execute( );
}

Texture::~Texture( ) { }

void Texture::unload( ) {
    stbi_image_free( contents );

    if ( device != nullptr && isLoadedToGPUMemory ) {
        vkDestroySampler( device, sampler, nullptr );
        vkDestroyImageView( device, imageView, nullptr );
        vkDestroyImage( device, textureGPUBuffer.buffer.image, nullptr );
        vkFreeMemory( device, textureGPUBuffer.memory, nullptr );
    }
}

END_NAMESPACES