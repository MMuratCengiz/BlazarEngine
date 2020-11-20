#pragma once

#include "../core/Common.h"
#include "../ecs/ISystem.h"
#include "../ecs/CMaterial.h"
#include "InstanceContext.h"
#include "CommandExecutor.h"
#include "RenderUtilities.h"

NAMESPACES( SomeVulkan, Graphics )

struct TextureObjectPart {
    vk::Sampler sampler{ };
    vk::ImageView imageView{ };
    vk::Image image;
    vma::Allocation allocation;
};

struct TextureObject {
    std::vector< TextureObjectPart > parts;
};

class STextureLoader : ECS::ISystem< ECS::CMaterial, TextureObject > {
private:
    std::unordered_map< std::string, TextureObjectPart > loadedTextures;
    std::shared_ptr< InstanceContext > context;
    std::shared_ptr< CommandExecutor > commandExecutor;
public:
    inline explicit STextureLoader( std::shared_ptr< InstanceContext > context, std::shared_ptr< CommandExecutor > commandExecutor )
    : context( std::move( context ) ), commandExecutor( std::move( commandExecutor ) ) { }

    void beforeFrame( TextureObject &input, const ECS::CMaterial & material ) override;
    ~STextureLoader();
private:
    void load( const ECS::Material::TextureInfo & texture );
    vk::SamplerCreateInfo texToSamplerCreateInfo( const uint32_t& mipLevels, const ECS::Material::TextureInfo &info );
    vk::Filter toVkFilter( const ECS::Material::Filter& filter );
    vk::SamplerAddressMode toVkAddressMode( const ECS::Material::AddressMode& filter );
    vk::SamplerMipmapMode toVkMipmapMode( const ECS::Material::MipmapMode& filter );
    void generateMipMaps( TextureObjectPart &part, const ECS::Material::TextureInfo &texture, int mipLevels, int width, int height ) const;
};

END_NAMESPACES
