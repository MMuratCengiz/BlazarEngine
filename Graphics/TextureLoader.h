#pragma once

#include "../Core/Common.h"
#include "../ECS/ISystem.h"
#include "../ECS/CMaterial.h"

#include "InstanceContext.h"
#include "CommandExecutor.h"
#include "RenderUtilities.h"

NAMESPACES( SomeVulkan, Graphics )

struct TextureBuffer {
    vk::Sampler sampler{ };
    vk::ImageView imageView{ };
    vk::Image image;
    vma::Allocation allocation;
};

struct TextureBufferList {
    std::vector< TextureBuffer > texturesObjects;
};

class TextureLoader {
protected:
    std::unordered_map< std::string, TextureBuffer > loadedTextures;
    std::shared_ptr< InstanceContext > context;
    std::shared_ptr< CommandExecutor > commandExecutor;
public:
    inline explicit TextureLoader( std::shared_ptr< InstanceContext > context, std::shared_ptr< CommandExecutor > commandExecutor )
    : context( std::move( context ) ), commandExecutor( std::move( commandExecutor ) ) { }

    void cache( const ECS::CMaterial & material );
    void load( TextureBufferList &input, const ECS::CMaterial & material );

    static vk::SamplerCreateInfo texToSamplerCreateInfo( const uint32_t& mipLevels, const ECS::Material::TextureInfo &info );
    static vk::Filter toVkFilter( const ECS::Material::Filter& filter );
    static vk::SamplerAddressMode toVkAddressMode( const ECS::Material::AddressMode& filter );
    static vk::SamplerMipmapMode toVkMipmapMode( const ECS::Material::MipmapMode& filter );

    ~TextureLoader();
private:
    void loadInner( const ECS::Material::TextureInfo & texture );
    void generateMipMaps( TextureBuffer &part, const ECS::Material::TextureInfo &texture, int mipLevels, int width, int height ) const;
};

END_NAMESPACES
