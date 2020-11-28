//
// Created by Murat on 11/28/2020.
//

#include "CubeMapLoader.h"

NAMESPACES( SomeVulkan, Graphics )

void CubeMapLoader::cache( ECS::CCubeMap &material ) {
    if ( loadedCubeMaps.find( getKey( material ) ) == loadedCubeMaps.end( ) ) {
        loadInner( material );
    }
}

void CubeMapLoader::load( TextureBuffer &output, ECS::CCubeMap &material ) {
    const auto &comparator = [ ]( const ECS::CubeMapSidePath &p1, const ECS::CubeMapSidePath &p2 ) -> bool {
        return static_cast< uint32_t >( p1.side ) > static_cast< uint32_t >( p2.side );
    };

    std::sort( material.texturePaths.begin( ), material.texturePaths.end( ), comparator );

    cache( material );
    TextureBuffer &buffer = loadedCubeMaps[ getKey( material ) ];

    output.allocation = buffer.allocation;
    output.imageView = buffer.imageView;
    output.sampler = buffer.sampler;
    output.image = buffer.image;
}

void CubeMapLoader::loadInner( ECS::CCubeMap &material ) {
    TextureBuffer &output = loadedCubeMaps[ getKey( material ) ];

    int width, height, channels;
    std::vector< std::pair< vk::Buffer, vma::Allocation > > stagingBuffers( material.texturePaths.size( ) );

    int mipStagingBufferIndex = 0;

    for ( const auto &cm: material.texturePaths ) {
        stbi_uc *contents = stbi_load( ( PATH( cm.path ) ).c_str( ), &width, &height, &channels, STBI_rgb_alpha );

        if ( contents == nullptr ) {
            TRACE( COMPONENT_TLOAD, VERBOSITY_CRITICAL, stbi_failure_reason( ) )

            throw std::runtime_error( "Couldn't find texture." );
        }

        std::pair< vk::Buffer, vma::Allocation > &stagingBuffer = stagingBuffers[ mipStagingBufferIndex++ ];

        RenderUtilities::initStagingBuffer( context, stagingBuffer, contents, width * height * 4 );

        stbi_image_free( contents );
    }

    vk::ImageCreateInfo imageCreateInfo { };

//    uint32_t mipLevels = material.texturePaths.size();

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
    output.image = imageAllocationPair.first;
    output.allocation = imageAllocationPair.second;

    vk::ImageViewCreateInfo imageViewCreateInfo { };

    imageViewCreateInfo.image = output.image;
    imageViewCreateInfo.viewType = vk::ImageViewType::eCube;
    imageViewCreateInfo.format = vk::Format::eR8G8B8A8Srgb;
    imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 6;

    output.imageView = context->logicalDevice.createImageView( imageViewCreateInfo );

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

    output.sampler = context->logicalDevice.createSampler( TextureLoader::texToSamplerCreateInfo( 1, cubeMapTextureInfo ) );


    int arrayLayer = 0;
    for ( auto &stagingBuffer: stagingBuffers ) {
        PipelineBarrierArgs toTransferOptimal { };

        toTransferOptimal.mipLevel = 1;
        toTransferOptimal.baseArrayLayer = arrayLayer;
        toTransferOptimal.layerCount = 1;
        toTransferOptimal.image = output.image;
        toTransferOptimal.oldLayout = vk::ImageLayout::eUndefined;
        toTransferOptimal.newLayout = vk::ImageLayout::eTransferDstOptimal;
        toTransferOptimal.sourceAccess = { };
        toTransferOptimal.destinationAccess = vk::AccessFlagBits::eTransferWrite;
        toTransferOptimal.sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        toTransferOptimal.destinationStage = vk::PipelineStageFlagBits::eTransfer;

        CopyBufferToImageArgs copyBufferToImageArgs { };
        copyBufferToImageArgs.image = output.image;
        copyBufferToImageArgs.sourceBuffer = stagingBuffer.first;
        copyBufferToImageArgs.width = width;
        copyBufferToImageArgs.height = height;
        copyBufferToImageArgs.arrayLayer = arrayLayer;
        copyBufferToImageArgs.mipLevel = 0;

        PipelineBarrierArgs toShaderOptimal { };

        toShaderOptimal.mipLevel = 1;
        toShaderOptimal.baseArrayLayer = arrayLayer;
        toShaderOptimal.layerCount = 1;
        toShaderOptimal.image = output.image;
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

std::string CubeMapLoader::getKey( const ECS::CCubeMap &material ) {


    std::stringstream keyGroup;

    for ( const auto &cm: material.texturePaths ) {
        keyGroup << cm.path;
    }

    return keyGroup.str( );
}

CubeMapLoader::~CubeMapLoader( ) {
    for ( auto &pair: loadedCubeMaps ) {
        context->logicalDevice.destroySampler( pair.second.sampler );
        context->logicalDevice.destroyImageView( pair.second.imageView );
        context->vma.destroyImage( pair.second.image, pair.second.allocation );
    }
}

END_NAMESPACES