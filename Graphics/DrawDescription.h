#pragma once

#include "../core/Common.h"
#include "../core/DynamicMemory.h"
#include "Texture.h"

NAMESPACES( SomeVulkan, Graphics )

class TextureLoader;

typedef enum AttributeCode {
    Vertices,
    TextureCoordinates,
    Normals,
    Indices,
    Textures // TODO should become material
} AttributeCode;

typedef struct AttributeDescription {
    uint32_t size;
} AttributeDescription;

struct DrawDescription {
    bool indexedMode = false;
    bool bufferedToGPU = false;

    float vertexCount;
    Core::DynamicMemory vertexMemory{ };

    std::vector< uint32_t > indices;
    std::vector< std::shared_ptr< TextureLoader > > textures;
};

class DrawDescriptionUtilities {
private:
    static inline std::unordered_map< AttributeCode, AttributeDescription > attributeDescriptions = {
            { AttributeCode::Vertices,           { 3 * sizeof( float ) } },
            { AttributeCode::TextureCoordinates, { 2 * sizeof( float ) } },
            { AttributeCode::Normals,            { 3 * sizeof( float ) } },
            { AttributeCode::Indices,            { 2 * sizeof( uint32_t ) } },
            { AttributeCode::Textures,           { 3 * sizeof( float ) } },
    };
public:
    static inline const AttributeDescription& getAttributeDescription( const AttributeCode& code ) {
        return attributeDescriptions[ code ];
    }
};

END_NAMESPACES
