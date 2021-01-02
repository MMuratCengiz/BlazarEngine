#pragma once

#include "VulkanContext.h"
#include "GLSLShaderSet.h"
#include "VulkanResourceProvider.h"
#include "VulkanSamplerAllocator.h"
#include "../GraphicsException.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct TextureDescription
{
    vk::ImageView imageView;
};

struct BindingUpdateInfo
{
    uint32_t index { };
    vk::DescriptorSet parent { };
    uint32_t arrayElement { };
};

struct UniformLocation
{
    bool found = false;
    uint32_t set { };
    uint32_t binding { };
};

struct DescriptorOrderInfo
{
    vk::DescriptorType type;
    std::string name;
    uint32_t set;
    int location;
};

struct PushConstantParent
{
    char *data { };
    uint32_t totalSize { };
    vk::ShaderStageFlagBits stage{ };
};

struct PushConstantBinding
{
    PushConstantParent parent;
    StructChild ref;
};

class DescriptorManager
{
private:
    static const uint32_t texturePreallocateCount;

    VulkanContext * context;
    std::shared_ptr< GLSLShaderSet > shaderSet;

    vk::Sampler sampler { };
    vk::ImageView imageView { };

    std::unordered_map< std::string, UniformLocation > uniformLocations;

    std::vector< std::vector< vk::DescriptorSet > > textureSets;
    std::vector< std::vector< vk::DescriptorSet > > uniformSets;

    std::vector< uint32_t > nextFreeTexture;
    std::vector< uint32_t > nextFreeUniform;
    std::vector< DescriptorOrderInfo > orders;

    std::unordered_map< std::string, std::vector< std::vector< vk::DescriptorSet > > > uniformSetMaps; // UniformName - ObjectIndex - FrameIndex
    std::unordered_map< std::string, std::vector< std::vector< vk::DescriptorSet > > > textureSetMaps; // UniformName - ObjectIndex - FrameIndex
    std::unordered_map< vk::ShaderStageFlagBits, std::vector< PushConstantParent > > pushConstantParents;
    std::unordered_map< std::string, std::vector< PushConstantBinding > > pushConstants;
    std::vector< vk::ShaderStageFlagBits > stagesWithPushConstants;
    std::vector< vk::DescriptorSetLayout > uniformLayouts;
    std::vector< vk::DescriptorSetLayout > textureLayouts;
    std::vector< vk::DescriptorSetLayout > layouts;

    vk::DescriptorPool uniformDescriptorPool;
    vk::DescriptorPool samplerDescriptorPool;

    uint32_t objectCounter;
public:
    explicit DescriptorManager( VulkanContext * context, std::shared_ptr< GLSLShaderSet > shaderSet );

    void updatePushConstant( const uint32_t &frameIndex, const std::string &uniformName, void *data );
    void updateUniform( const uint32_t &frameIndex, const std::string &uniformName, const std::pair< vk::Buffer, vma::Allocation > &buffer,
                        const uint32_t &objectIndex, const int &arrayIndex = -1 );

    void updateTexture( const uint32_t &frameIndex, const std::string &uniformName, const VulkanTextureWrapper &buffer, const uint32_t &objectIndex );

    vk::DescriptorSet &getUniformDescriptorSet( const uint32_t &frameIndex, const std::string &uniformName, const uint32_t &objectIndex, const uint32_t &arrayIndex = -1 );
    vk::DescriptorSet &getTextureDescriptorSet( const uint32_t &frameIndex, const std::string &uniformName, const uint32_t &objectIndex, const uint32_t &arrayIndex = 0 );

    std::vector< vk::DescriptorSet > getOrderedSets( const uint32_t &frame, const uint32_t &objectIndex );
    std::vector< PushConstantParent > getPushConstantBindings( const uint32_t &frame );

    const std::vector< vk::DescriptorSetLayout > &getLayouts( );

    inline void incrementObjectCounter( ) noexcept { objectCounter++; }
    inline void resetObjectCounter( ) noexcept { objectCounter = 0; }
    inline uint32_t getObjectCount( ) const noexcept { return objectCounter; }
    ~DescriptorManager( );
private:

    void createDescriptorPool( );

    void ensureTextureHasDescriptor( const uint32_t &frameIndex, const std::string &texturePath, const uint32_t &objectIndex );
    void ensureUniformHasDescriptor( const uint32_t &frameIndex, const std::string &uniformName, const uint32_t &objectIndex, const int &arrayIndex );

    static std::string getUniformKey( const std::string &uniformName, const int &arrayIndex = -1 );
    vk::WriteDescriptorSet getCommonWriteDescriptorSet( const UniformLocation &uniformLocation, const BindingUpdateInfo &updateInfo );

    void addUniformDescriptorSet( const std::string &uniformName, UniformLocation &location, vk::DescriptorSetLayout &layout, const uint32_t &objectIndex );
    void addTextureDescriptorSet( const std::string &uniformName, UniformLocation &location, vk::DescriptorSetLayout &layout, const uint32_t &objectIndex );
};

END_NAMESPACES

