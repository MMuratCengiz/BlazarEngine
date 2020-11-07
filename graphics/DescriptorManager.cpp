//
// Created by Murat on 11/1/2020.
//

#include "DescriptorManager.h"


NAMESPACES( SomeVulkan, Graphics )

DescriptorManager::DescriptorManager( const std::shared_ptr< RenderContext >& context,
                                      const std::shared_ptr< ShaderLayout >& shaderLayout ) :
                                      context( context ), shaderLayout( shaderLayout ) {
    createDescriptorSets();
}

void DescriptorManager::createDescriptorSets( ) {
    uint32_t swapChainImageCount = context->swapChainImages.size();

    vk::DescriptorSetLayoutCreateInfo createInfo { };

    const std::vector< DescriptorSetBinding > &descriptorSetBindings = shaderLayout->getDescriptorSetBindings( );
    vk::DescriptorSetLayoutBinding vkDescriptorSetBindings[descriptorSetBindings.size( )];

    for ( uint8_t i = 0; i < shaderLayout->getDescriptorCount( ); ++i ) {
        vkDescriptorSetBindings[ i ] = descriptorSetBindings[ i ].binding;
    }

    createInfo.bindingCount = shaderLayout->getDescriptorCount( );
    createInfo.pBindings = vkDescriptorSetBindings;

    context->descriptorSetLayout = context->logicalDevice.createDescriptorSetLayout( createInfo );

    std::vector< vk::DescriptorSetLayout > layouts( swapChainImageCount, context->descriptorSetLayout );
    vk::DescriptorSetAllocateInfo allocateInfo { };
    allocateInfo.descriptorPool = context->descriptorPool;
    allocateInfo.descriptorSetCount = swapChainImageCount;
    allocateInfo.pSetLayouts = layouts.data( );

    context->descriptorSets = context->logicalDevice.allocateDescriptorSets( allocateInfo );
}

void DescriptorManager::updateUniformDescriptorSetBinding( const BindingUpdateInfo &updateInfo ) {
    const DescriptorSetBinding& ref = shaderLayout->getDescriptorSetBindings()[ updateInfo.index ];

    vk::DescriptorBufferInfo descriptorBufferInfo { };

    vk::WriteDescriptorSet writeDescriptorSet = getCommonWriteDescriptorSet( updateInfo );

    descriptorBufferInfo.buffer = updateInfo.memory.buffer.regular;
    descriptorBufferInfo.offset = 0;
    descriptorBufferInfo.range = ref.size;

    writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
    writeDescriptorSet.pImageInfo = nullptr;
    writeDescriptorSet.pTexelBufferView = nullptr;

    context->logicalDevice.updateDescriptorSets( 1, &writeDescriptorSet, 0, nullptr );
}

void DescriptorManager::updateTextureDescriptorSetBinding( const TextureBindingUpdateInfo &texUpdateInfo ) {
    const BindingUpdateInfo& updateInfo = texUpdateInfo.updateInfo;

    vk::DescriptorImageInfo descriptorImageInfo { };
    vk::WriteDescriptorSet writeDescriptorSet = getCommonWriteDescriptorSet( updateInfo );

    descriptorImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    descriptorImageInfo.imageView = texUpdateInfo.texture->getImageView();
    descriptorImageInfo.sampler =  texUpdateInfo.texture->getSampler();

    writeDescriptorSet.pImageInfo = &descriptorImageInfo;
    writeDescriptorSet.pBufferInfo = nullptr;
    writeDescriptorSet.pTexelBufferView = nullptr;

    context->logicalDevice.updateDescriptorSets( 1, &writeDescriptorSet, 0, nullptr );
}

vk::WriteDescriptorSet DescriptorManager::getCommonWriteDescriptorSet( const BindingUpdateInfo &updateInfo ) {
    const DescriptorSetBinding& ref = shaderLayout->getDescriptorSetBindings()[ updateInfo.index ];
    vk::WriteDescriptorSet writeDescriptorSet { };

    writeDescriptorSet.dstSet = updateInfo.parent;
    writeDescriptorSet.dstBinding = ref.index;
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.descriptorType = ref.type;
    writeDescriptorSet.descriptorCount = 1;

    return writeDescriptorSet;
}

DescriptorManager::~DescriptorManager( ) {
    context->logicalDevice.destroySampler( sampler );
    context->logicalDevice.destroyImageView( imageView );
    context->logicalDevice.destroyDescriptorSetLayout( context->descriptorSetLayout );
}

END_NAMESPACES