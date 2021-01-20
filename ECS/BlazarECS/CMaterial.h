#pragma once

#include <BlazarCore/Common.h>
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

NAMESPACE( Material )

enum class Filter
{
    Nearest,
    Linear,
    CubicIMG,
    CubicEXT
};

enum class AddressMode
{
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder,
    MirrorClampToEdge,
    MirrorClampToEdgeKHR
};

enum class MipmapMode
{
    eNearest,
    eLinear
};

enum class ImageFormat
{
    R8G8B8A8Unorm,
    R8G8B8Unorm,
    R8G8Unorm,
    R8Unorm,
};

struct InMemoryTexture
{
    unsigned char * contents;
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    ImageFormat format;
};

struct TextureInfo
{
    std::string path;

    bool isInMemory = false;
    InMemoryTexture inMemoryTexture;

    Filter magFilter = Filter::Linear;
    Filter minFilter = Filter::Linear;
    AddressMode U = AddressMode::Repeat;
    AddressMode V = AddressMode::Repeat;
    AddressMode W = AddressMode::Repeat;
    MipmapMode mipmapMode = MipmapMode::eLinear;
    float mipLodBias = 0.0f;
    float minLod = 0.0f;
    float maxLod = 0.0f;
};

struct TextureScaleOptions
{
    bool scaleX = false;
    bool scaleY = false;
    bool scaleZ = false;
};

END_NAMESPACE

struct CMaterial : public IComponent
{
public:
    Material::TextureScaleOptions textureScaleOptions;
    std::vector< Material::TextureInfo > textures;
    Material::TextureInfo heightMap{ };
    glm::vec4 diffuse { };
    glm::vec4 specular { };
    float shininess { };

    BLAZAR_COMPONENT( CMaterial )
};

END_NAMESPACES