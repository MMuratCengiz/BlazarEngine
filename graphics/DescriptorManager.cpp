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

    VkDescriptorSetLayoutCreateInfo createInfo { };

    const std::vector< DescriptorSetBinding > &descriptorSetBindings = shaderLayout->getDescriptorSetBindings( );
    VkDescriptorSetLayoutBinding vkDescriptorSetBindings[descriptorSetBindings.size( )];

    for ( uint8_t i = 0; i < shaderLayout->getDescriptorCount( ); ++i ) {
        vkDescriptorSetBindings[ i ] = descriptorSetBindings[ i ].binding;
    }

    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = shaderLayout->getDescriptorCount( );
    createInfo.pBindings = vkDescriptorSetBindings;

    if ( vkCreateDescriptorSetLayout( context->logicalDevice, &createInfo, nullptr, &context->descriptorSetLayout ) !=
         VK_SUCCESS ) {
        throw GraphicsException( GraphicsException::Source::RenderSurface, "Couldn't create descriptor set layout! " );
    }

    std::vector< VkDescriptorSetLayout > layouts( swapChainImageCount, context->descriptorSetLayout );
    VkDescriptorSetAllocateInfo allocateInfo { };

    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = context->descriptorPool;
    allocateInfo.descriptorSetCount = swapChainImageCount;
    allocateInfo.pSetLayouts = layouts.data( );

    context->descriptorSets.resize( swapChainImageCount );

    if ( vkAllocateDescriptorSets( context->logicalDevice, &allocateInfo, context->descriptorSets.data( ) ) !=
         VK_SUCCESS ) {
        throw GraphicsException( GraphicsException::Source::RenderSurface, "Couldn't create descriptor sets! " );
    }
}

void DescriptorManager::updateUniformDescriptorSetBinding( const BindingUpdateInfo &updateInfo ) {
    const DescriptorSetBinding& ref = shaderLayout->getDescriptorSetBindings()[ updateInfo.index ];

    VkDescriptorBufferInfo descriptorBufferInfo { };

    VkWriteDescriptorSet writeDescriptorSet = getCommonWriteDescriptorSet( updateInfo );

    descriptorBufferInfo.buffer = updateInfo.memory.buffer.regular;
    descriptorBufferInfo.offset = 0;
    descriptorBufferInfo.range = ref.size;

    writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
    writeDescriptorSet.pImageInfo = nullptr;
    writeDescriptorSet.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets( this->context->logicalDevice, 1, &writeDescriptorSet, 0, nullptr );
}

void DescriptorManager::updateTextureDescriptorSetBinding( const TextureBindingUpdateInfo &texUpdateInfo ) {
    const BindingUpdateInfo& updateInfo = texUpdateInfo.updateInfo;

    VkDescriptorImageInfo descriptorImageInfo { };
    VkWriteDescriptorSet writeDescriptorSet = getCommonWriteDescriptorSet( updateInfo );

    descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    descriptorImageInfo.imageView = texUpdateInfo.texture->getImageView();
    descriptorImageInfo.sampler =  texUpdateInfo.texture->getSampler();

    writeDescriptorSet.pImageInfo = &descriptorImageInfo;
    writeDescriptorSet.pBufferInfo = nullptr;
    writeDescriptorSet.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets( this->context->logicalDevice, 1, &writeDescriptorSet, 0, nullptr );
}

VkWriteDescriptorSet DescriptorManager::getCommonWriteDescriptorSet( const BindingUpdateInfo &updateInfo ) {
    const DescriptorSetBinding& ref = shaderLayout->getDescriptorSetBindings()[ updateInfo.index ];
    VkWriteDescriptorSet writeDescriptorSet { };

    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = updateInfo.parent;
    writeDescriptorSet.dstBinding = ref.index;
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.descriptorType = ref.type;
    writeDescriptorSet.descriptorCount = 1;

    return writeDescriptorSet;
}

DescriptorManager::~DescriptorManager( ) {
    vkDestroySampler( context->logicalDevice, sampler, nullptr );
    vkDestroyImageView( context->logicalDevice, imageView, nullptr );

    vkDestroyDescriptorSetLayout( context->logicalDevice, context->descriptorSetLayout, nullptr );
}

END_NAMESPACES