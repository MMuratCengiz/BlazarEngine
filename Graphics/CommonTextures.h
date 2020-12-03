#pragma once

#include "../core/Common.h"
#include "../graphics/DrawDescription.h"
#include "Texture.h"

NAMESPACES( SomeVulkan, Graphics )

typedef enum class TextureID {
    TutTexture
} TextureID;

class CommonTextures {
private:
    static inline std::unordered_map< TextureID, std::shared_ptr< MaterialLoader > > textures = {
//            { TextureID::TutTexture, std::make_shared< Texture >( 2, "/assets/textures/texture.jpg" )}
    };

public:
    static std::shared_ptr< MaterialLoader > getTexture( TextureID id ) {
        return textures[ id ];
    }
};

END_NAMESPACES