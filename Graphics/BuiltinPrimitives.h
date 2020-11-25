#pragma once

#include "../Core/Common.h"
#include "../Core/DynamicMemory.h"

NAMESPACES( SomeVulkan, Graphics )

enum class PrimitiveType {
    Cube
};

class CubePrimitive {
    Core::DynamicMemory vertexBuffer;
public:
    CubePrimitive( ) {
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

class BuiltinPrimitives {
public:
    static std::string getPrimitivePath( PrimitiveType type ) {
        switch ( type ) {
            case PrimitiveType::Cube:
                return "BuiltinPrimitives/Cube";
                break;
        }

        return "";
    }
};

END_NAMESPACES
