#include "DescriptorManager.h"

#include <utility>


NAMESPACES( ENGINE_NAMESPACE, Graphics )

const uint32_t DescriptorManager::texturePreallocateCount = 12;

DescriptorManager::DescriptorManager( std::shared_ptr< InstanceContext > context, std::shared_ptr< GLSLShaderSet > shaderSet ) : context( std::move( context ) ), shaderSet( std::move( shaderSet ) )
{
    createDescriptorPool( );

    uniformLayouts.resize( this->shaderSet->getDescriptorSets( ).size( ) );
    textureLayouts.resize( uniformLayouts.size( ) );
    layouts.resize( uniformLayouts.size( ) );

    for ( const auto &sets: this->shaderSet->getDescriptorSets( ) )
    {
        uint32_t setIndex = sets.id;

        for ( const auto &bindings: sets.descriptorSetBindings )
        {
            uniformLocations[ bindings.name ] = UniformLocation {
                    true, setIndex, bindings.index
            };
        }

        const DescriptorSet &set = this->shaderSet->getDescriptorSetBySetId( setIndex );

        vk::DescriptorSetLayoutCreateInfo createInfo { };

        std::vector< vk::DescriptorSetLayoutBinding > vkDescriptorSetBindings = set.descriptorSetLayoutBindings;

        createInfo.bindingCount = vkDescriptorSetBindings.size( );
        createInfo.pBindings = vkDescriptorSetBindings.data( );

        if ( vkDescriptorSetBindings[ 0 ].descriptorType == vk::DescriptorType::eCombinedImageSampler )
        {
            textureLayouts[ setIndex ] = this->context->logicalDevice.createDescriptorSetLayout( createInfo );
            uniformLayouts[ setIndex ] = nullptr;
            layouts[ setIndex ] = textureLayouts[ setIndex ];
        } else
        {
            textureLayouts[ setIndex ] = nullptr;
            uniformLayouts[ setIndex ] = this->context->logicalDevice.createDescriptorSetLayout( createInfo );
            layouts[ setIndex ] = uniformLayouts[ setIndex ];
        }
    }

    expandTextureDescriptorSets( );
}

void DescriptorManager::addUniformDescriptorSet( const std::string &uniformName, UniformLocation &location, vk::DescriptorSetLayout &layout )
{
    uint32_t swapChainImageCount = context->swapChainImages.size( );

    std::vector< vk::DescriptorSetLayout > layoutsPtr { swapChainImageCount, layout };

    vk::DescriptorSetAllocateInfo allocateInfo { };
    allocateInfo.descriptorPool = samplerDescriptorPool;
    allocateInfo.descriptorSetCount = layoutsPtr.size( );
    allocateInfo.pSetLayouts = layoutsPtr.data( );

    uniformSetMaps[ uniformName ] = context->logicalDevice.allocateDescriptorSets( allocateInfo );
}

void DescriptorManager::expandTextureDescriptorSets( )
{
    uint32_t swapChainImageCount = context->swapChainImages.size( );

    if ( textureSets.empty( ) )
    {
        nextFreeTexture.resize( swapChainImageCount, 0 );
        textureSets.resize( swapChainImageCount );
        textureSetMaps.resize( swapChainImageCount );
    }

    for ( auto &layout: textureLayouts )
    {
        if ( layout == nullptr )
        {
            continue;
        }

        std::vector< vk::DescriptorSetLayout > layoutsPtr { swapChainImageCount, layout };

        for ( auto &textureSet : textureSets )
        {
            vk::DescriptorSetAllocateInfo allocateInfo { };
            allocateInfo.descriptorPool = samplerDescriptorPool;
            allocateInfo.descriptorSetCount = layoutsPtr.size( );
            allocateInfo.pSetLayouts = layoutsPtr.data( );

            const auto &newSets = context->logicalDevice.allocateDescriptorSets( allocateInfo );
            textureSet.insert( textureSet.end( ), newSets.begin( ), newSets.end( ) );
        }
    }
}

// Todo create more pools as necessary
void DescriptorManager::createDescriptorPool( )
{
    auto swapChainImageCount = static_cast< uint32_t >( context->swapChainImages.size( ) );

    vk::DescriptorPoolSize uniformPoolSize { };
    uniformPoolSize.type = vk::DescriptorType::eUniformBuffer;
    uniformPoolSize.descriptorCount = swapChainImageCount;

    vk::DescriptorPoolCreateInfo uniformDescriptorPoolCreateInfo { };
    uniformDescriptorPoolCreateInfo.poolSizeCount = 1;
    uniformDescriptorPoolCreateInfo.pPoolSizes = &uniformPoolSize;
    uniformDescriptorPoolCreateInfo.maxSets = swapChainImageCount * 120;

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

vk::WriteDescriptorSet DescriptorManager::getCommonWriteDescriptorSet( const UniformLocation &uniformLocation, const BindingUpdateInfo &updateInfo )
{
    const DescriptorSet &set = shaderSet->getDescriptorSetBySetId( uniformLocation.set );

    const DescriptorSetBinding &ref = set.descriptorSetBindings[ updateInfo.index ];
    vk::WriteDescriptorSet writeDescriptorSet { };

    writeDescriptorSet.dstSet = updateInfo.parent;
    writeDescriptorSet.dstBinding = uniformLocation.binding;
    writeDescriptorSet.dstArrayElement = updateInfo.arrayElement;
    writeDescriptorSet.descriptorType = ref.type;
    writeDescriptorSet.descriptorCount = 1;

    return writeDescriptorSet;
}

void DescriptorManager::ensureTextureHasDescriptor( const uint32_t &frameIndex, const std::string &texturePath )
{
    if ( textureSetMaps[ frameIndex ].find( texturePath ) == textureSetMaps[ frameIndex ].end( ) )
    {
        if ( nextFreeTexture[ frameIndex ] >= textureSets[ frameIndex ].size( ) )
        {
            expandTextureDescriptorSets( );
        }

        textureSetMaps[ frameIndex ][ texturePath ] = textureSets[ frameIndex ][ nextFreeTexture[ frameIndex ] ];
        nextFreeTexture[ frameIndex ]++;
    }
}

void DescriptorManager::ensureUniformHasDescriptor( const uint32_t &frameIndex, const std::string &uniformName, const int &arrayIndex )
{
    const std::string &key = getUniformKey( uniformName, arrayIndex );

    if ( uniformSetMaps.find( key ) == uniformSetMaps.end( ) )
    {
        auto &uniformSet = uniformLocations[ uniformName ];
        auto &layout = uniformLayouts[ uniformSet.set ];

        addUniformDescriptorSet( uniformName, uniformSet, layout );
    }
}

vk::DescriptorSet &DescriptorManager::getUniformDescriptorSet( const uint32_t &frameIndex, const std::string &uniformName, const uint32_t &arrayIndex )
{
    ensureUniformHasDescriptor( frameIndex, uniformName, arrayIndex );

    return uniformSetMaps[ getUniformKey( uniformName, arrayIndex ) ][ frameIndex ];
}

vk::DescriptorSet &DescriptorManager::getTextureDescriptorSet( const uint32_t &frameIndex, const std::string &texturePath, const uint32_t &textureIndex )
{
    ensureTextureHasDescriptor( frameIndex, texturePath );

    return textureSetMaps[ frameIndex ][ texturePath ];
}

void DescriptorManager::updateUniform( const uint32_t &frameIndex, const std::string &uniformName, const std::pair< vk::Buffer, vma::Allocation > &buffer, const int &arrayIndex )
{
    ensureUniformHasDescriptor( frameIndex, uniformName, arrayIndex );

    const std::string &key = getUniformKey( uniformName, arrayIndex );

    UniformLocation &uniformLocation = uniformLocations[ key ];

    BindingUpdateInfo updateInfo {
            uniformLocation.binding,
            uniformSetMaps[ key ][ frameIndex ],
            arrayIndex < 0 ? 0 : uint32_t( arrayIndex )
    };

    const DescriptorSet &set = shaderSet->getDescriptorSetBySetId( uniformLocation.set );
    const DescriptorSetBinding &ref = set.descriptorSetBindings[ uniformLocation.binding ];
    vk::WriteDescriptorSet writeDescriptorSet = getCommonWriteDescriptorSet( uniformLocation, updateInfo );

    vk::DescriptorBufferInfo descriptorBufferInfo { };
    descriptorBufferInfo.buffer = buffer.first;
    descriptorBufferInfo.offset = 0;
    descriptorBufferInfo.range = ref.size;

    writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
    writeDescriptorSet.pImageInfo = nullptr;
    writeDescriptorSet.pTexelBufferView = nullptr;

    context->logicalDevice.updateDescriptorSets( 1, &writeDescriptorSet, 0, nullptr );
}

bool DescriptorManager::existsSetForTexture( const uint32_t &frameIndex, const std::string &path )
{
    return textureSetMaps[ frameIndex ].find( path ) != textureSetMaps[ frameIndex ].end( );
}

void DescriptorManager::updateTexture( const uint32_t &frameIndex, const std::string &path, const TextureBuffer &buffer )
{
    ensureTextureHasDescriptor( frameIndex, path );

    // Todo + texture index when texture arrays are implemented
    const std::string &key = getUniformKey( Core::Constants::getConstant( Core::ConstantName::ShaderInputSampler ), 1 );

    UniformLocation &uniformLocation = uniformLocations[ key ];
    BindingUpdateInfo updateInfo {
            uniformLocation.binding,
            textureSetMaps[ frameIndex ][ path ]
    };

    const DescriptorSet &set = shaderSet->getDescriptorSetBySetId( uniformLocation.set );
    const DescriptorSetBinding &ref = set.descriptorSetBindings[ uniformLocation.binding ];
    vk::WriteDescriptorSet writeDescriptorSet = getCommonWriteDescriptorSet( uniformLocation, updateInfo );

    vk::DescriptorImageInfo descriptorImageInfo { };
    descriptorImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    descriptorImageInfo.imageView = buffer.imageView;
    descriptorImageInfo.sampler = buffer.sampler;

    writeDescriptorSet.pImageInfo = &descriptorImageInfo;
    writeDescriptorSet.pBufferInfo = nullptr;
    writeDescriptorSet.pTexelBufferView = nullptr;

    context->logicalDevice.updateDescriptorSets( 1, &writeDescriptorSet, 0, nullptr );
}

const std::vector< vk::DescriptorSetLayout > &DescriptorManager::getLayouts( )
{
    return layouts;
}

std::string DescriptorManager::getUniformKey( const std::string &uniformName, const int &arrayIndex )
{
    if ( arrayIndex == -1 )
    {
        return uniformName;
    }

    std::stringstream key;
    key << uniformName << arrayIndex;
    return key.str( );
}

DescriptorManager::~DescriptorManager( )
{
    context->logicalDevice.destroySampler( sampler );
    context->logicalDevice.destroyImageView( imageView );
    for ( auto &layout: layouts )
    {
        context->logicalDevice.destroyDescriptorSetLayout( layout );
    }
    context->logicalDevice.destroyDescriptorPool( uniformDescriptorPool );
    context->logicalDevice.destroyDescriptorPool( samplerDescriptorPool );
}

END_NAMESPACES