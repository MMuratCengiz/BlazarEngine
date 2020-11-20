#pragma once

#include "../core/Common.h"
#include "InstanceContext.h"
#include "GLSLShaderSet.h"
#include "GraphicsException.h"
#include "RendererTypes.h"
#include "STextureLoader.h"

NAMESPACES( SomeVulkan, Graphics )

struct TextureDescription {
    vk::ImageView imageView;
};

struct BindingUpdateInfo {
    uint32_t index{};
    vk::DescriptorSet parent{};
    DeviceBuffer buffer;
};

struct TextureBindingUpdateInfo {
    BindingUpdateInfo updateInfo { };
    TextureObjectPart texture{ };
};

class InstanceContext;

class DescriptorManager {
private:
    std::shared_ptr< InstanceContext > context;
    std::shared_ptr< GLSLShaderSet > shaderSet;

    vk::Sampler sampler{ };
    vk::ImageView imageView{ };
public:
    explicit DescriptorManager( const std::shared_ptr< InstanceContext >& renderContext, const std::shared_ptr< GLSLShaderSet>& shaderSet );

    void updateUniformDescriptorSetBinding( const BindingUpdateInfo& updateInfo );
    void updateTextureDescriptorSetBinding( const TextureBindingUpdateInfo& updateInfo );

    DescriptorManager( const DescriptorManager & ) = delete;
    DescriptorManager &operator=( const DescriptorManager & ) = delete;

    ~DescriptorManager();
private:
    void createDescriptorSets( );
    vk::WriteDescriptorSet getCommonWriteDescriptorSet( const BindingUpdateInfo &updateInfo );
};

END_NAMESPACES

