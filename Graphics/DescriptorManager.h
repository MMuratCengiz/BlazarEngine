#pragma once

#include "../Core/Common.h"
#include "InstanceContext.h"
#include "GLSLShaderSet.h"
#include "GraphicsException.h"
#include "TextureLoader.h"

NAMESPACES( SomeVulkan, Graphics )

struct TextureDescription {
    vk::ImageView imageView;
};

struct BindingUpdateInfo {
    uint32_t index{};
    vk::DescriptorSet parent{};
};

class InstanceContext;

struct UniformLocation {
    bool found = false;
    uint32_t set{};
    uint32_t binding{};
};

#define ViewProjectionSetName "ViewProjection"
#define TextureSetNamePrefix std::string( "Texture" )

class DescriptorManager {
private:
    static const uint32_t texturePreallocateCount;

    std::shared_ptr< InstanceContext > context;
    std::shared_ptr< GLSLShaderSet > shaderSet;

    vk::Sampler sampler{ };
    vk::ImageView imageView{ };

    UniformLocation viewProjectionLocation;
    UniformLocation firstTextureLocation;

    std::vector< vk::DescriptorSet > viewProjectionSets;
    std::vector< std::vector< vk::DescriptorSet > > textureSets;
    std::vector< uint32_t > nextFreeTexture;
    std::vector< std::unordered_map< std::string, vk::DescriptorSet > > textureSetMaps;
    std::vector< vk::DescriptorSetLayout > layouts;
    vk::DescriptorPool uniformDescriptorPool;
    vk::DescriptorPool samplerDescriptorPool;
public:
    explicit DescriptorManager( std::shared_ptr< InstanceContext >  renderContext, std::shared_ptr< GLSLShaderSet>  shaderSet );

    void updateViewProjection( const uint32_t& frameIndex, const std::pair< vk::Buffer, vma::Allocation >& buffer );
    bool existsSetForTexture( const uint32_t& frameIndex, const std::string& path );
    void updateTexture( const uint32_t& frameIndex, const std::string& path, const TextureBuffer& buffer );
    vk::DescriptorSet& getViewProjectionDescriptorSet( const uint32_t& frameIndex );
    vk::DescriptorSet& getTextureDescriptorSet( const uint32_t& frameIndex, const std::string& texturePath, const uint32_t& textureIndex );
    const std::vector< vk::DescriptorSetLayout >& getLayouts( );

    ~DescriptorManager();
private:
    void createViewProjectionDescriptorSets( );
    void expandTextureDescriptorSets( );
    void createDescriptorPool( );
    vk::WriteDescriptorSet getCommonWriteDescriptorSet( const UniformLocation& uniformLocation, const BindingUpdateInfo &updateInfo );
    void ensureTextureHasDescriptor( const uint32_t &frameIndex, const std::string &texturePath );
};

END_NAMESPACES

