#pragma once

#include "../Core/Common.h"
#include "InstanceContext.h"
#include "GLSLShaderSet.h"
#include "GraphicsException.h"
#include "MaterialLoader.h"

NAMESPACES( SomeVulkan, Graphics )

struct TextureDescription {
    vk::ImageView imageView;
};

struct BindingUpdateInfo {
    uint32_t index{};
    vk::DescriptorSet parent{};
    uint32_t arrayElement{ };
};

struct UniformLocation {
    bool found = false;
    uint32_t set{};
    uint32_t binding{};
};

class DescriptorManager {
private:
    static const uint32_t texturePreallocateCount;

    std::shared_ptr< InstanceContext > context;
    std::shared_ptr< GLSLShaderSet > shaderSet;

    vk::Sampler sampler{ };
    vk::ImageView imageView{ };

    std::unordered_map< std::string, UniformLocation > uniformLocations;

    std::vector< std::vector< vk::DescriptorSet > > textureSets;
    std::vector< std::vector< vk::DescriptorSet > > uniformSets;

    std::vector< uint32_t > nextFreeTexture;
    std::vector< uint32_t > nextFreeUniform;

    std::vector< std::unordered_map< std::string, vk::DescriptorSet > > textureSetMaps;
    std::unordered_map< std::string, std::vector< vk::DescriptorSet > > uniformSetMaps;

    std::vector< vk::DescriptorSetLayout > uniformLayouts;
    std::vector< vk::DescriptorSetLayout > textureLayouts;
    std::vector< vk::DescriptorSetLayout > layouts;

    vk::DescriptorPool uniformDescriptorPool;
    vk::DescriptorPool samplerDescriptorPool;
public:
    explicit DescriptorManager( std::shared_ptr< InstanceContext >  renderContext, std::shared_ptr< GLSLShaderSet>  shaderSet );

    bool existsSetForTexture( const uint32_t& frameIndex, const std::string& path );
    void updateUniform( const uint32_t& frameIndex, const std::string& uniformName, const std::pair< vk::Buffer, vma::Allocation >& buffer, const int& arrayIndex = -1 );
    void updateTexture( const uint32_t& frameIndex, const std::string& path, const TextureBuffer& buffer );

    vk::DescriptorSet& getUniformDescriptorSet( const uint32_t& frameIndex, const std::string& uniformName, const uint32_t& arrayIndex = -1 );
    vk::DescriptorSet& getTextureDescriptorSet( const uint32_t& frameIndex, const std::string& texturePath, const uint32_t& textureIndex );

    const std::vector< vk::DescriptorSetLayout >& getLayouts( );
    ~DescriptorManager();
private:

    void createDescriptorPool( );

//    void expandUniformDescriptorSets( );
    void expandTextureDescriptorSets( );

    void ensureTextureHasDescriptor( const uint32_t &frameIndex, const std::string &texturePath );
    void ensureUniformHasDescriptor( const uint32_t &frameIndex, const std::string& uniformName, const int& arrayIndex );

    static std::string getUniformKey( const std::string& uniformName, const int& arrayIndex = -1 );
    vk::WriteDescriptorSet getCommonWriteDescriptorSet( const UniformLocation& uniformLocation, const BindingUpdateInfo &updateInfo );
    void addUniformDescriptorSet( const std::string &uniformName, UniformLocation &location, vk::DescriptorSetLayout &layout );
};

END_NAMESPACES

