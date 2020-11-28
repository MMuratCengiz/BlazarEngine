#pragma once

#include "../Core/Common.h"
#include "../Core/DynamicMemory.h"

NAMESPACES( SomeVulkan, Graphics )

enum class PrimitiveType {
    LightedCube,
    PlainCube
};

class LightedCubePrimitive {
    Core::DynamicMemory vertexBuffer;
public:
    LightedCubePrimitive( ) {
        vertexBuffer.setInitialSize( 180 * sizeof( float ) );

        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, } );
        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, } );
        vertexBuffer.attachElements< float >( { -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, } );
        vertexBuffer.attachElements< float >( { -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, } );
        vertexBuffer.attachElements< float >( { -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, } );
        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, } );
        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, } );
        vertexBuffer.attachElements< float >( { 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, } );
        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, } );
        vertexBuffer.attachElements< float >( { -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, } );
        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, } );
        vertexBuffer.attachElements< float >( { 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, } );
        vertexBuffer.attachElements< float >( { 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, } );
        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, } );
        vertexBuffer.attachElements< float >( { 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, } );
        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, } );
        vertexBuffer.attachElements< float >( { -1.0f, 1.0f, -1.0f, 1.0f, 0.0f, } );
        vertexBuffer.attachElements< float >( { -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, } );
        vertexBuffer.attachElements< float >( { -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, } );
        vertexBuffer.attachElements< float >( { 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, } );
        vertexBuffer.attachElements< float >( { 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, } );
        vertexBuffer.attachElements< float >( { 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, } );
        vertexBuffer.attachElements< float >( { -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, } );
        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, } );
        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, } );
        vertexBuffer.attachElements< float >( { 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, 1.0f, 0.0f, 1.0f } );

    } // Todo maybe set flags telling if normals should be added or not

    uint32_t getVertexCount( ) {
        return 36;
    }

    [[nodiscard]] const Core::DynamicMemory &getVertexBuffer( ) const {
        return vertexBuffer;
    }
};

class PlainCubePrimitive {
    Core::DynamicMemory vertexBuffer;
public:
    PlainCubePrimitive( ) {
        vertexBuffer.setInitialSize( 108 * sizeof( float ) );

        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, -1.0f } );
        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, 1.0f  } );
        vertexBuffer.attachElements< float >( { -1.0f, 1.0f, 1.0f   } );
        vertexBuffer.attachElements< float >( { -1.0f, 1.0f, 1.0f   } );
        vertexBuffer.attachElements< float >( { -1.0f, 1.0f, -1.0f  } );
        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, -1.0f } );
        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, -1.0f } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, -1.0f   } );
        vertexBuffer.attachElements< float >( { 1.0f, -1.0f, -1.0f  } );
        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, -1.0f } );
        vertexBuffer.attachElements< float >( { -1.0f, 1.0f, -1.0f  } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, -1.0f   } );
        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, -1.0f } );
        vertexBuffer.attachElements< float >( { 1.0f, -1.0f, -1.0f  } );
        vertexBuffer.attachElements< float >( { 1.0f, -1.0f, 1.0f   } );
        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, -1.0f } );
        vertexBuffer.attachElements< float >( { 1.0f, -1.0f, 1.0f   } );
        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, 1.0f  } );
        vertexBuffer.attachElements< float >( { -1.0f, 1.0f, -1.0f  } );
        vertexBuffer.attachElements< float >( { -1.0f, 1.0f, 1.0f   } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, 1.0f    } );
        vertexBuffer.attachElements< float >( { -1.0f, 1.0f, -1.0f  } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, 1.0f    } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, -1.0f   } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, -1.0f   } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, 1.0f    } );
        vertexBuffer.attachElements< float >( { 1.0f, -1.0f, 1.0f   } );
        vertexBuffer.attachElements< float >( { 1.0f, -1.0f, 1.0f   } );
        vertexBuffer.attachElements< float >( { 1.0f, -1.0f, -1.0f  } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, -1.0f   } );
        vertexBuffer.attachElements< float >( { -1.0f, 1.0f, 1.0f   } );
        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, 1.0f  } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, 1.0f    } );
        vertexBuffer.attachElements< float >( { -1.0f, -1.0f, 1.0f  } );
        vertexBuffer.attachElements< float >( { 1.0f, -1.0f, 1.0f   } );
        vertexBuffer.attachElements< float >( { 1.0f, 1.0f, 1.0f    } );

    }

    uint32_t getVertexCount( ) {
        return 36;
    }

    [[nodiscard]] const Core::DynamicMemory &getVertexBuffer( ) const {
        return vertexBuffer;
    }
};

class BuiltinPrimitives {
public:
    static std::string getPrimitivePath( PrimitiveType type ) {
        switch ( type ) {
            case PrimitiveType::LightedCube:
                return "BuiltinPrimitives/LightedCube";
                break;
            case PrimitiveType::PlainCube:
                return "BuiltinPrimitives/PlainCube";
                break;
        }

        return "";
    }
};

END_NAMESPACES
