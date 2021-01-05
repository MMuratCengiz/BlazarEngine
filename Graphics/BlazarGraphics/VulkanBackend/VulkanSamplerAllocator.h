#pragma once

#include <BlazarECS/ECS.h>
#include "VulkanContext.h"
#include "VulkanCommandExecutor.h"
#include "VulkanUtilities.h"
#include "../AssetManager.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct ShaderInputMaterial
{
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;

    float shininess;
};

struct TextureLoadArguments
{
    VulkanContext* context;
    std::shared_ptr< VulkanCommandExecutor > commandExecutor;

    std::shared_ptr< SamplerDataAttachment > image;
};

class VulkanSamplerAllocator
{
private:
    VulkanSamplerAllocator( ) = default;
public:
    static void load( const TextureLoadArguments &arguments, VulkanTextureWrapper * target );

    static vk::SamplerCreateInfo texToSamplerCreateInfo( const uint32_t &mipLevels, const ECS::Material::TextureInfo &info );
    static vk::Filter toVkFilter( const ECS::Material::Filter &filter );
    static vk::SamplerAddressMode toVkAddressMode( const ECS::Material::AddressMode &filter );
    static vk::SamplerMipmapMode toVkMipmapMode( const ECS::Material::MipmapMode &filter );
private:
    static void generateMipMaps( const TextureLoadArguments &arguments, VulkanTextureWrapper * target, int mipLevels, int width, int height );
};

END_NAMESPACES
