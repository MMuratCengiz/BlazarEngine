//
// Created by Murat on 11/1/2020.
//

#include "Texture.h"
#include "RenderUtilities.h"

using namespace SomeVulkan::Graphics;

Texture::Texture( uint8_t dimension, const std::string& path, TextureInfo textureInfo )
        : dimension( dimension ), path( path ), textureInfo( textureInfo ) {
    int lWidth, lHeight;

    contents = stbi_load( ( PATH( path ) ).c_str( ), &lWidth, &lHeight, &channels,
                          STBI_rgb_alpha );

    if ( contents == nullptr ) {
        TRACE( COMPONENT_TLOAD, VERBOSITY_CRITICAL, stbi_failure_reason( ) )

        throw std::runtime_error( "Couldn't find texture.");
    }

    width = static_cast< uint32_t >( lWidth );
    height = static_cast< uint32_t >( lHeight );
}

uint32_t Texture::size( ) const {
    return getWidth( ) * getHeight( ) * getChannels( ) * sizeof( stbi_uc );
}

void Texture::loadIntoGPUMemory( std::shared_ptr< RenderContext > &context, pCommandExecutor &commandExecutor ) {
    if ( isLoadedToGPUMemory ) {
        return;
    }

    this->device = context->logicalDevice;

    VkImageCreateInfo imageCreateInfo { };

    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    if ( vkCreateImage( context->logicalDevice, &imageCreateInfo, nullptr, &textureGPUBuffer.buffer.image ) !=
         VK_SUCCESS ) {
        throw GraphicsException( GraphicsException::Source::Renderer, "Failed to create image!" );
    }

    VkMemoryRequirements memoryRequirements { };

    RenderUtilities::allocateImageMemory( context, textureGPUBuffer.buffer.image, textureGPUBuffer.memory, memoryRequirements );

    vkBindImageMemory( context->logicalDevice, textureGPUBuffer.buffer.image, textureGPUBuffer.memory, 0 );

    isLoadedToGPUMemory = true;

    VkImageViewCreateInfo imageViewCreateInfo { };

    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = textureGPUBuffer.buffer.image;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
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
    samplerCreateInfo.maxLod = textureInfo.maxLod;

    if ( vkCreateSampler( context->logicalDevice, &samplerCreateInfo, nullptr, &sampler ) != VK_SUCCESS ) {
        throw GraphicsException { GraphicsException::Source::RenderSurface, "Could not create image view!" };
    }

    RenderUtilities::copyToDeviceMemory(
            context->logicalDevice,
            textureGPUBuffer.memory,
            ( void * ) contents,
            memoryRequirements.size
    );

    commandExecutor->startCommandExecution( )
            ->generateBuffers( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, 1 )
            ->beginCommand( )
            ->pipelineBarrier( textureGPUBuffer.buffer.image, VK_IMAGE_LAYOUT_PREINITIALIZED,
                               VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )
            ->execute( );

}

Texture::~Texture( ) { }

void Texture::unload() {
    stbi_image_free( contents );

    if ( device != nullptr && isLoadedToGPUMemory ) {
        vkDestroySampler( device, sampler, nullptr );
        vkDestroyImageView( device, imageView, nullptr );
        vkDestroyImage( device, textureGPUBuffer.buffer.image, nullptr );
        vkFreeMemory( device, textureGPUBuffer.memory, nullptr );
    }
}
