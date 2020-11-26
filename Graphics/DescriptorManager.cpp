//
// Created by Murat on 11/1/2020.
//

#include "DescriptorManager.h"

#include <utility>


NAMESPACES( SomeVulkan, Graphics )

const uint32_t DescriptorManager::texturePreallocateCount = 12;

DescriptorManager::DescriptorManager( std::shared_ptr< InstanceContext > context, std::shared_ptr< GLSLShaderSet > shaderSet ) :
        context( std::move( context ) ), shaderSet( std::move( shaderSet ) ) {
    createDescriptorPool();
    uint32_t setIndex = 0;

    layouts.resize( this->shaderSet->getDescriptorSets( ).size( ) );

    for ( const auto &sets: this->shaderSet->getDescriptorSets( ) ) {
        for ( const auto &bindings: sets.descriptorSetBindings ) {
            if ( bindings.name == ViewProjectionSetName ) {
                viewProjectionLocation.set = setIndex;
                viewProjectionLocation.binding = bindings.index;
            }

            if ( bindings.name == ( TextureSetNamePrefix + "1" ) ) {
                firstTextureLocation.set = setIndex;
                firstTextureLocation.binding = bindings.index;
            }
        }

        const DescriptorSet &set = this->shaderSet->getDescriptorSetBySetId( setIndex );

        vk::DescriptorSetLayoutCreateInfo createInfo { };

        std::vector< vk::DescriptorSetLayoutBinding > vkDescriptorSetBindings = set.descriptorSetLayoutBindings;

        createInfo.bindingCount = vkDescriptorSetBindings.size( );
        createInfo.pBindings = vkDescriptorSetBindings.data( );

        layouts[ setIndex ] = this->context->logicalDevice.createDescriptorSetLayout( createInfo );

        ++setIndex;
    }

    createViewProjectionDescriptorSets( );
    expandTextureDescriptorSets( );
}

void DescriptorManager::createViewProjectionDescriptorSets( ) {
    uint32_t swapChainImageCount = context->swapChainImages.size( );
    viewProjectionSets.resize( swapChainImageCount );


    std::vector< vk::DescriptorSetLayout > layoutsPtr { swapChainImageCount, layouts[ viewProjectionLocation.set ] };
    vk::DescriptorSetAllocateInfo allocateInfo { };
    allocateInfo.descriptorPool = uniformDescriptorPool;
    allocateInfo.descriptorSetCount = layoutsPtr.size( );
    allocateInfo.pSetLayouts = layoutsPtr.data( );

    viewProjectionSets = context->logicalDevice.allocateDescriptorSets( allocateInfo );
}

void DescriptorManager::expandTextureDescriptorSets( ) {
    uint32_t swapChainImageCount = context->swapChainImages.size( );


    if ( textureSets.empty( ) ) {
        nextFreeTexture.resize( swapChainImageCount, 0 );
        textureSets.resize( swapChainImageCount );
        textureSetMaps.resize( swapChainImageCount );
    }

    std::vector< vk::DescriptorSetLayout > layoutsPtr { texturePreallocateCount, layouts[ firstTextureLocation.set ] };
    vk::DescriptorSetAllocateInfo allocateInfo { };

    for ( auto &textureSet : textureSets ) {
        layoutsPtr = { texturePreallocateCount, layouts[ firstTextureLocation.set ] };
        allocateInfo.descriptorPool = samplerDescriptorPool;
        allocateInfo.descriptorSetCount = layoutsPtr.size( );
        allocateInfo.pSetLayouts = layoutsPtr.data( );

        const auto &newSets = context->logicalDevice.allocateDescriptorSets( allocateInfo );
        textureSet.insert( textureSet.end( ), newSets.begin( ), newSets.end( ) );
    }
}

// Todo create more pools as necessary
void DescriptorManager::createDescriptorPool( ) {

    auto swapChainImageCount = static_cast< uint32_t >( context->swapChainImages.size( ) );

    vk::DescriptorPoolSize uniformPoolSize { };
    uniformPoolSize.type = vk::DescriptorType::eUniformBuffer;
    uniformPoolSize.descriptorCount = swapChainImageCount;

    vk::DescriptorPoolCreateInfo uniformDescriptorPoolCreateInfo { };
    uniformDescriptorPoolCreateInfo.poolSizeCount = 1;
    uniformDescriptorPoolCreateInfo.pPoolSizes = &uniformPoolSize;
    uniformDescriptorPoolCreateInfo.maxSets = swapChainImageCount + swapChainImageCount * 12;

    uniformDescriptorPool = context->logicalDevice.createDescriptorPool( uniformDescriptorPoolCreateInfo );

    vk::DescriptorPoolSize samplerPoolSize { };
    samplerPoolSize.type = vk::DescriptorType::eCombinedImageSampler;
    samplerPoolSize.descriptorCount = swapChainImageCount;

    vk::DescriptorPoolCreateInfo samplerDescriptorPoolCreateInfo { };
    samplerDescriptorPoolCreateInfo.poolSizeCount = 1;
    samplerDescriptorPoolCreateInfo.pPoolSizes = &samplerPoolSize;
    samplerDescriptorPoolCreateInfo.maxSets = swapChainImageCount * 12;

    samplerDescriptorPool = context->logicalDevice.createDescriptorPool( samplerDescriptorPoolCreateInfo );
}

vk::WriteDescriptorSet DescriptorManager::getCommonWriteDescriptorSet( const UniformLocation &uniformLocation, const BindingUpdateInfo &updateInfo ) {
    const DescriptorSet &set = shaderSet->getDescriptorSetBySetId( uniformLocation.set );

    const DescriptorSetBinding &ref = set.descriptorSetBindings[ updateInfo.index ];
    vk::WriteDescriptorSet writeDescriptorSet { };

    writeDescriptorSet.dstSet = updateInfo.parent;
    writeDescriptorSet.dstBinding = uniformLocation.binding;
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.descriptorType = ref.type;
    writeDescriptorSet.descriptorCount = 1;

    return writeDescriptorSet;
}

vk::DescriptorSet &DescriptorManager::getViewProjectionDescriptorSet( const uint32_t &frameIndex ) {
    return viewProjectionSets[ frameIndex ];
}

void DescriptorManager::ensureTextureHasDescriptor( const uint32_t &frameIndex, const std::string &texturePath ) {
    if ( textureSetMaps[ frameIndex ].find( texturePath ) == textureSetMaps[ frameIndex ].end() ) {
        if ( nextFreeTexture[ frameIndex ] >= textureSets[ frameIndex ].size() ) {
            expandTextureDescriptorSets();
        }

        textureSetMaps[ frameIndex ][ texturePath ] = textureSets[ frameIndex][ nextFreeTexture[ frameIndex ] ];
        nextFreeTexture[ frameIndex ]++;
    }
}

vk::DescriptorSet &DescriptorManager::getTextureDescriptorSet( const uint32_t &frameIndex, const std::string &texturePath, const uint32_t &textureIndex ) {
    ensureTextureHasDescriptor( frameIndex, texturePath );

    return textureSetMaps[ frameIndex ][ texturePath ];
}

void DescriptorManager::updateViewProjection( const uint32_t &frameIndex, const std::pair< vk::Buffer, vma::Allocation > &buffer ) {
    BindingUpdateInfo updateInfo {
            viewProjectionLocation.binding,
            viewProjectionSets[ frameIndex ]
    };

    const DescriptorSet &set = shaderSet->getDescriptorSetBySetId( viewProjectionLocation.set );
    const DescriptorSetBinding &ref = set.descriptorSetBindings[ viewProjectionLocation.binding ];
    vk::WriteDescriptorSet writeDescriptorSet = getCommonWriteDescriptorSet( viewProjectionLocation, updateInfo );

    vk::DescriptorBufferInfo descriptorBufferInfo { };
    descriptorBufferInfo.buffer = buffer.first;
    descriptorBufferInfo.offset = 0;
    descriptorBufferInfo.range = ref.size;

    writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
    writeDescriptorSet.pImageInfo = nullptr;
    writeDescriptorSet.pTexelBufferView = nullptr;

    context->logicalDevice.updateDescriptorSets( 1, &writeDescriptorSet, 0, nullptr );
}

bool DescriptorManager::existsSetForTexture( const uint32_t &frameIndex, const std::string &path ) {
    return textureSetMaps[ frameIndex ].find( path ) != textureSetMaps[ frameIndex ].end() ;
}

void DescriptorManager::updateTexture( const uint32_t &frameIndex, const std::string &path, const TextureBuffer &buffer ) {
    ensureTextureHasDescriptor( frameIndex, path );

    BindingUpdateInfo updateInfo {
            firstTextureLocation.binding, // Todo + texture index when texture arrays are implemented
            textureSetMaps[ frameIndex ][ path ]
    };

    const DescriptorSet &set = shaderSet->getDescriptorSetBySetId( firstTextureLocation.set );
    const DescriptorSetBinding &ref = set.descriptorSetBindings[ firstTextureLocation.binding ];
    vk::WriteDescriptorSet writeDescriptorSet = getCommonWriteDescriptorSet( firstTextureLocation, updateInfo );

    vk::DescriptorImageInfo descriptorImageInfo { };
    descriptorImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    descriptorImageInfo.imageView = buffer.imageView;
    descriptorImageInfo.sampler = buffer.sampler;

    writeDescriptorSet.pImageInfo = &descriptorImageInfo;
    writeDescriptorSet.pBufferInfo = nullptr;
    writeDescriptorSet.pTexelBufferView = nullptr;

    context->logicalDevice.updateDescriptorSets( 1, &writeDescriptorSet, 0, nullptr );
}

const std::vector< vk::DescriptorSetLayout > &DescriptorManager::getLayouts( ) {
    return layouts;
}

DescriptorManager::~DescriptorManager( ) {
    context->logicalDevice.destroySampler( sampler );
    context->logicalDevice.destroyImageView( imageView );
    for ( auto &layout: layouts ) {
        context->logicalDevice.destroyDescriptorSetLayout( layout );
    }
    context->logicalDevice.destroyDescriptorPool( uniformDescriptorPool );
    context->logicalDevice.destroyDescriptorPool( samplerDescriptorPool );
}

END_NAMESPACES