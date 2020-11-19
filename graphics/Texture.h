#pragma once

#include "../core/Common.h"
#include "RendererTypes.h"
#include "CommandExecutor.h"
#include "../ecs/CMaterial.h"

NAMESPACES( SomeVulkan, Graphics )

typedef struct AddressMode {
    vk::SamplerAddressMode U;
    vk::SamplerAddressMode V;
    vk::SamplerAddressMode W;
} AddressMode;

typedef struct TextureInfo {
    vk::Filter magFilter;
    vk::Filter minFilter;
    AddressMode addressMode;
    vk::SamplerMipmapMode mipmapMode;
    float mipLodBias;
    float minLod;
    float maxLod;
} TextureInfo;

class InstanceContext; // There is a circular dependency here

class TextureLoader {
private:

    bool isLoadedToGPUMemory{};

    uint32_t mipLevels;
    uint8_t dimension;
    uint32_t width;
    uint32_t height;
    int32_t channels { };
    stbi_uc *contents;

    std::string path;
    TextureInfo textureInfo;

    vk::Image image{ };
    vma::Allocation imageAllocation{ };
    vk::Sampler sampler{ };
    vk::ImageView imageView{ };
    vk::Device device;
    std::shared_ptr< InstanceContext > context;
public:
    explicit TextureLoader( uint8_t dimension, const std::string& path, TextureInfo sampler = defaultTextureInfo( ) );
    void unload();
    ~TextureLoader( );

    inline bool loaded( ) const { return contents != nullptr; }
    [[nodiscard]] inline const uint8_t &getDimension( ) const { return dimension; };
    [[nodiscard]] inline const uint32_t &getWidth( ) const { return width; }
    [[nodiscard]] inline const uint32_t &getHeight( ) const { return height; };
    [[nodiscard]] inline const int &getChannels( ) const { return channels; };
    [[nodiscard]] inline const stbi_uc *data( ) const { return contents; };
    [[nodiscard]] inline const TextureInfo &getTextureInfo( ) const { return textureInfo; };
    [[nodiscard]] inline const bool &isLoadedIntoGPUMemory( ) const { return isLoadedToGPUMemory; };
    [[nodiscard]] inline const vk::Sampler &getSampler( ) const { return sampler; };
    [[nodiscard]] inline const vk::ImageView &getImageView( ) const { return imageView; };
    [[nodiscard]] inline const vma::Allocation &getAllocation( ) const { return imageAllocation; };
    [[nodiscard]] inline const vk::Image &getImage( ) const { return image; };

    [[nodiscard]] uint32_t size( ) const;
    void loadIntoGPUMemory( std::shared_ptr< InstanceContext > &context,
                            std::shared_ptr< CommandExecutor > &commandExecutor );


    inline static TextureInfo defaultTextureInfo( ) {
        TextureInfo texInfo { };

        texInfo.magFilter = vk::Filter::eLinear;
        texInfo.minFilter = vk::Filter::eLinear;
        texInfo.addressMode.U = vk::SamplerAddressMode::eRepeat;
        texInfo.addressMode.V = vk::SamplerAddressMode::eRepeat;
        texInfo.addressMode.W = vk::SamplerAddressMode::eRepeat;
        texInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
        texInfo.mipLodBias = 0.0f;
        texInfo.minLod = 0.0f;
        texInfo.maxLod = 0.0f;

        return texInfo;
    }
private:
    void generateMipMaps( std::shared_ptr< InstanceContext > &context, std::shared_ptr< CommandExecutor > &commandExecutor ) const;
};

END_NAMESPACES