#pragma once

#include "../core/Common.h"
#include "Texture.h"
#include "RenderContext.h"
#include "ShaderLayout.h"
#include "GraphicsException.h"
#include "RendererTypes.h"

NAMESPACES( SomeVulkan, Graphics )

class Texture;

typedef struct TextureDescription {
    VkImageView imageView;
} TextureDescription;

typedef struct BindingUpdateInfo {
    uint32_t index{};
    VkDescriptorSet parent{};
    DeviceMemory memory;
} BindingUpdateInfo;

typedef struct TextureBindingUpdateInfo {
    BindingUpdateInfo updateInfo { };
    std::shared_ptr< Texture > texture;
} TextureBindingUpdateInfo;

class RenderContext;

class DescriptorManager {
private:
    std::shared_ptr< RenderContext > context;
    std::shared_ptr< ShaderLayout > shaderLayout;

    VkSampler sampler{ };
    VkImageView imageView{ };
public:
    explicit DescriptorManager( const std::shared_ptr< RenderContext >& renderContext,
                       const std::shared_ptr< ShaderLayout >& shaderLayout );

    void updateUniformDescriptorSetBinding( const BindingUpdateInfo& updateInfo );
    void updateTextureDescriptorSetBinding( const TextureBindingUpdateInfo& updateInfo );

    DescriptorManager( const DescriptorManager & ) = delete;
    DescriptorManager &operator=( const DescriptorManager & ) = delete;

    ~DescriptorManager();
private:
    void createDescriptorSets( );
    VkWriteDescriptorSet getCommonWriteDescriptorSet( const BindingUpdateInfo &updateInfo );
};

END_NAMESPACES

