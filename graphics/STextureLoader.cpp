//
// Created by Murat on 11/19/2020.
//

#include "STextureLoader.h"

NAMESPACES( SomeVulkan, Graphics )

void SomeVulkan::Graphics::STextureLoader::beforeFrame( TextureObject &input, const ECS::CMaterial &material ) {
    for ( const auto &texture: material.textures ) {
        if ( loadedTextures.find( texture.path ) == loadedTextures.end( ) ) {
            load( texture );
        }

        TextureObjectPart &part = loadedTextures[ texture.path ];
        TextureObjectPart &partToCopyTo = input.parts.emplace_back( TextureObjectPart { } );

        partToCopyTo.image = part.image;
        partToCopyTo.sampler = part.sampler;
        partToCopyTo.imageView = part.imageView;
    }
}

void SomeVulkan::Graphics::STextureLoader::load( const ECS::Material::TextureInfo &texture ) {
    loadedTextures[ texture.path ] = TextureObjectPart { };
    TextureObjectPart &part = loadedTextures[ texture.path ];

    int width, height, channels;

    stbi_uc *contents = stbi_load( ( PATH( texture.path ) ).c_str( ), &width, &height, &channels, STBI_rgb_alpha );

    if ( contents == nullptr ) {
        TRACE( COMPONENT_TLOAD, VERBOSITY_CRITICAL, stbi_failure_reason( ) )

        throw std::runtime_error( "Couldn't find texture." );
    }

    int mipLevels = std::floor( std::log2( std::max( width, height ) ) ) + 1;

    std::pair< vk::Buffer, vma::Allocation > stagingBuffer;

    RenderUtilities::initStagingBuffer( context, stagingBuffer, contents, width * height * 4 );

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
    part.image = imageAllocationPair.first;
    part.allocation = imageAllocationPair.second;

    vk::ImageViewCreateInfo imageViewCreateInfo { };

    imageViewCreateInfo.image = part.image;
    imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
    imageViewCreateInfo.format = vk::Format::eR8G8B8A8Srgb;
    imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = mipLevels;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    part.imageView = context->logicalDevice.createImageView( imageViewCreateInfo );
    part.sampler = context->logicalDevice.createSampler( texToSamplerCreateInfo( mipLevels, texture ) );

    PipelineBarrierArgs args { };

    args.mipLevel = mipLevels;
    args.image = part.image;
    args.oldLayout = vk::ImageLayout::eUndefined;
    args.newLayout = vk::ImageLayout::eTransferDstOptimal;
    args.sourceAccess = { };
    args.destinationAccess = vk::AccessFlagBits::eTransferWrite;
    args.sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
    args.destinationStage = vk::PipelineStageFlagBits::eTransfer;

    CopyBufferToImageArgs copyBufferToImageArgs { };
    copyBufferToImageArgs.image = part.image;
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

    if ( ( properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear ) != vk::FormatFeatureFlagBits::eSampledImageFilterLinear ) {
        throw std::runtime_error( "Unsupported device, VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT must be  supported" );
    }

    generateMipMaps( part, texture, mipLevels, width, height );
    stbi_image_free( contents );
}

void STextureLoader::generateMipMaps( TextureObjectPart &part, const ECS::Material::TextureInfo &texture, int mipLevels, int width, int height ) const {
    int32_t mipWidth = width, mipHeight = height;

    auto cmdBuffer = commandExecutor->startCommandExecution( )->generateBuffers( vk::CommandBufferUsageFlagBits::eOneTimeSubmit, 1 );

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
        pipelineBarrierArgs.image = part.image;
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
        blitArgs.sourceImage = part.image;
        blitArgs.destinationImage = part.image;
        blitArgs.sourceImageLayout = vk::ImageLayout::eTransferSrcOptimal;
        blitArgs.destinationImageLayout = vk::ImageLayout::eTransferDstOptimal;

        cmdBuffer->blitImage( blitArgs );

        /* After the blit transition the image to the form that will be used by the shader */

        pipelineBarrierArgs.baseMipLevel = index - 1;
        pipelineBarrierArgs.mipLevel = 1;
        pipelineBarrierArgs.image = part.image;
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
    pipelineBarrierArgs.image = part.image;
    pipelineBarrierArgs.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    pipelineBarrierArgs.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    pipelineBarrierArgs.sourceAccess = vk::AccessFlagBits::eTransferRead;
    pipelineBarrierArgs.destinationAccess = vk::AccessFlagBits::eShaderRead;
    pipelineBarrierArgs.sourceStage = vk::PipelineStageFlagBits::eTransfer;
    pipelineBarrierArgs.destinationStage = vk::PipelineStageFlagBits::eFragmentShader;

    cmdBuffer->pipelineBarrier( pipelineBarrierArgs );
    cmdBuffer->execute( );
}

vk::SamplerCreateInfo SomeVulkan::Graphics::STextureLoader::texToSamplerCreateInfo( const uint32_t &mipLevels, const SomeVulkan::ECS::Material::TextureInfo &info ) {
    vk::SamplerCreateInfo samplerCreateInfo { };

    samplerCreateInfo.magFilter = toVkFilter( info.magFilter );
    samplerCreateInfo.minFilter = toVkFilter( info.minFilter );
    samplerCreateInfo.addressModeU = toVkAddressMode( info.U );
    samplerCreateInfo.addressModeV = toVkAddressMode( info.V );
    samplerCreateInfo.addressModeW = toVkAddressMode( info.W );
    samplerCreateInfo.anisotropyEnable = VK_TRUE;
    samplerCreateInfo.maxAnisotropy = 16.0f;
    samplerCreateInfo.borderColor = vk::BorderColor::eFloatOpaqueBlack;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.compareOp = vk::CompareOp::eAlways;
    samplerCreateInfo.mipmapMode = toVkMipmapMode( info.mipmapMode );
    samplerCreateInfo.mipLodBias = info.mipLodBias;
    samplerCreateInfo.minLod = info.minLod;
    samplerCreateInfo.maxLod = mipLevels;

    return samplerCreateInfo;
}

vk::Filter SomeVulkan::Graphics::STextureLoader::toVkFilter( const SomeVulkan::ECS::Material::Filter &filter ) {
    switch ( filter ) {
        case ECS::Material::Filter::Nearest:
            return vk::Filter::eNearest;
        case ECS::Material::Filter::Linear:
            return vk::Filter::eLinear;
        case ECS::Material::Filter::CubicIMG:
            return vk::Filter::eCubicIMG;
        case ECS::Material::Filter::CubicEXT:
            return vk::Filter::eCubicEXT;
    }

    return vk::Filter::eLinear;
}

vk::SamplerAddressMode SomeVulkan::Graphics::STextureLoader::toVkAddressMode( const SomeVulkan::ECS::Material::AddressMode &addressMode ) {
    switch ( addressMode ) {
        case ECS::Material::AddressMode::Repeat:
            return vk::SamplerAddressMode::eRepeat;
        case ECS::Material::AddressMode::MirroredRepeat:
            return vk::SamplerAddressMode::eMirroredRepeat;
        case ECS::Material::AddressMode::ClampToEdge:
            return vk::SamplerAddressMode::eClampToEdge;
        case ECS::Material::AddressMode::ClampToBorder:
            return vk::SamplerAddressMode::eClampToBorder;
        case ECS::Material::AddressMode::MirrorClampToEdge:
            return vk::SamplerAddressMode::eMirrorClampToEdge;
        case ECS::Material::AddressMode::MirrorClampToEdgeKHR:
            return vk::SamplerAddressMode::eMirrorClampToEdgeKHR;
    }
    return vk::SamplerAddressMode::eClampToBorder;
}

vk::SamplerMipmapMode SomeVulkan::Graphics::STextureLoader::toVkMipmapMode( const SomeVulkan::ECS::Material::MipmapMode &mipmapMode ) {
    switch( mipmapMode ) {
        case ECS::Material::MipmapMode::eNearest:
            return vk::SamplerMipmapMode::eNearest;
        case ECS::Material::MipmapMode::eLinear:
            return vk::SamplerMipmapMode::eLinear;
    }

    return vk::SamplerMipmapMode::eLinear;
}

STextureLoader::~STextureLoader( ) {
    for ( auto& pair: loadedTextures ) {
        context->logicalDevice.destroySampler( pair.second.sampler );
        context->logicalDevice.destroyImageView( pair.second.imageView );
        context->vma.destroyImage( pair.second.image, pair.second.allocation );
    }
}

END_NAMESPACES