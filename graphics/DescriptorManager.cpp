//
// Created by Murat on 11/1/2020.
//

#include "DescriptorManager.h"


NAMESPACES( SomeVulkan, Graphics )

DescriptorManager::DescriptorManager( const std::shared_ptr< InstanceContext >& context,
    const std::shared_ptr< GLSLShaderSet>& shaderSet ) :
                                      context( context ), shaderSet( shaderSet ) {
    createDescriptorSets();
}

void DescriptorManager::createDescriptorSets( ) {
    uint32_t swapChainImageCount = context->swapChainImages.size();

    vk::DescriptorSetLayoutCreateInfo createInfo { };

    const std::vector< DescriptorSet > &descriptorSetBindings = shaderSet->getDescriptorSets( );

    std::vector< vk::DescriptorSetLayoutBinding > vkDescriptorSetBindings = shaderSet->getDescriptorSetBySetId( 0 ).descriptorSetLayoutBindings;

    createInfo.bindingCount = vkDescriptorSetBindings.size();
    createInfo.pBindings = vkDescriptorSetBindings.data();

    context->descriptorSetLayout = context->logicalDevice.createDescriptorSetLayout( createInfo );

    std::vector< vk::DescriptorSetLayout > layouts( swapChainImageCount, context->descriptorSetLayout );
    vk::DescriptorSetAllocateInfo allocateInfo { };
    allocateInfo.descriptorPool = context->descriptorPool;
    allocateInfo.descriptorSetCount = swapChainImageCount;
    allocateInfo.pSetLayouts = layouts.data( );

    context->descriptorSets = context->logicalDevice.allocateDescriptorSets( allocateInfo );
}

void DescriptorManager::updateUniformDescriptorSetBinding( const BindingUpdateInfo &updateInfo ) {
    const DescriptorSetBinding& ref = shaderSet->getDescriptorSetBySetId( 0 ).descriptorSetBindings[ updateInfo.index ];

    vk::DescriptorBufferInfo descriptorBufferInfo { };

    vk::WriteDescriptorSet writeDescriptorSet = getCommonWriteDescriptorSet( updateInfo );

    descriptorBufferInfo.buffer = updateInfo.buffer.regular;
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
    descriptorImageInfo.imageView = texUpdateInfo.texture.imageView;
    descriptorImageInfo.sampler =  texUpdateInfo.texture.sampler;

    writeDescriptorSet.pImageInfo = &descriptorImageInfo;
    writeDescriptorSet.pBufferInfo = nullptr;
    writeDescriptorSet.pTexelBufferView = nullptr;

    context->logicalDevice.updateDescriptorSets( 1, &writeDescriptorSet, 0, nullptr );
}

vk::WriteDescriptorSet DescriptorManager::getCommonWriteDescriptorSet( const BindingUpdateInfo &updateInfo ) {
    const DescriptorSetBinding& ref = shaderSet->getDescriptorSetBySetId( 0 ).descriptorSetBindings [ updateInfo.index ];
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
}

END_NAMESPACES