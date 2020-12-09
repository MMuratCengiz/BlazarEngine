#pragma once

#include <BlazarECS/ECS.h>
#include "InstanceContext.h"
#include "CommandExecutor.h"
#include "RenderUtilities.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct ShaderInputMaterial
{
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;

    float shininess;
};

struct TextureBuffer
{
    vk::Sampler sampler { };
    vk::ImageView imageView { };
    vk::Image image;
    vma::Allocation allocation;
};

struct MaterialBuffer
{
    std::vector< TextureBuffer > texturesObjects;
    std::pair< vk::Buffer, vma::Allocation > buffer;
};

class MaterialLoader
{
protected:
    std::unordered_map< std::string, TextureBuffer > loadedTextures;
    std::unordered_map< uint64_t, std::pair< vk::Buffer, vma::Allocation > > loadedMaterials;
    std::shared_ptr< InstanceContext > context;
    std::shared_ptr< CommandExecutor > commandExecutor;
public:
    inline explicit MaterialLoader( std::shared_ptr< InstanceContext > context, std::shared_ptr< CommandExecutor > commandExecutor )
            : context( std::move( context ) ), commandExecutor( std::move( commandExecutor ) )
    { }

    void cache( const ECS::CMaterial &material );
    void load( MaterialBuffer &input, const ECS::CMaterial &material );

    static vk::SamplerCreateInfo texToSamplerCreateInfo( const uint32_t &mipLevels, const ECS::Material::TextureInfo &info );
    static vk::Filter toVkFilter( const ECS::Material::Filter &filter );
    static vk::SamplerAddressMode toVkAddressMode( const ECS::Material::AddressMode &filter );
    static vk::SamplerMipmapMode toVkMipmapMode( const ECS::Material::MipmapMode &filter );

    ~MaterialLoader( );
private:
    void loadInner( const ECS::Material::TextureInfo &texture );
    void generateMipMaps( TextureBuffer &part, const ECS::Material::TextureInfo &texture, int mipLevels, int width, int height ) const;
    void loadMaterial( const ECS::CMaterial &material );
};

END_NAMESPACES
