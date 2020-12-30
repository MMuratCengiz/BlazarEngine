#pragma once

#include <BlazarCore/Common.h>
#include <BlazarCore/DynamicMemory.h>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

enum class PrimitiveType
{
    LightedCube,
    PlainCube
};

class LitCubePrimitive
{
    std::vector< float > vertices;
public:
    LitCubePrimitive( )
    {
        // Left Cube, Triangle 1
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, } );
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, } );
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, } );
        // Left Cube, Triangle 2
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, } );
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, } );
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, } );
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // Back Cube, Triangle 1
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, } );
        vertices.insert( vertices.end( ), { 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, } );
        // Back Cube, Triangle 2
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, } );
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, } );
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // Bottom Cube, Triangle 1
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, } );
        vertices.insert( vertices.end( ), { 1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, } );
        vertices.insert( vertices.end( ), { 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, } );
        // Bottom Cube, Triangle 2
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, } );
        vertices.insert( vertices.end( ), { 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, } );
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, } );
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // Top Cube, Triangle 1
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, } );
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, } );
        // Top Cube, Triangle 2
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, } );
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // Right Cube, Triangle 1
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, } );
        vertices.insert( vertices.end( ), { 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, } );
        // Right Cube, Triangle 2
        vertices.insert( vertices.end( ), { 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, } );
        vertices.insert( vertices.end( ), { 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, } );
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // Front Cube, Triangle 1
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, } );
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, } );
        // Front Cube, Triangle 2
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, } );
        vertices.insert( vertices.end( ), { 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f } );
    }

    uint32_t getVertexCount( )
    {
        return 36;
    }

    [[nodiscard]] const std::vector< float > &getVertices( ) const
    {
        return vertices;
    }
};

class PlainCubePrimitive
{
    std::vector< float > vertices;
public:
    PlainCubePrimitive( )
    {
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, -1.0f } );
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, 1.0f } );
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, 1.0f } );
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, 1.0f } );
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, -1.0f } );
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, -1.0f } );
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, -1.0f } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, -1.0f } );
        vertices.insert( vertices.end( ), { 1.0f, -1.0f, -1.0f } );
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, -1.0f } );
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, -1.0f } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, -1.0f } );
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, -1.0f } );
        vertices.insert( vertices.end( ), { 1.0f, -1.0f, -1.0f } );
        vertices.insert( vertices.end( ), { 1.0f, -1.0f, 1.0f } );
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, -1.0f } );
        vertices.insert( vertices.end( ), { 1.0f, -1.0f, 1.0f } );
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, 1.0f } );
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, -1.0f } );
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, 1.0f } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, 1.0f } );
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, -1.0f } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, 1.0f } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, -1.0f } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, -1.0f } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, 1.0f } );
        vertices.insert( vertices.end( ), { 1.0f, -1.0f, 1.0f } );
        vertices.insert( vertices.end( ), { 1.0f, -1.0f, 1.0f } );
        vertices.insert( vertices.end( ), { 1.0f, -1.0f, -1.0f } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, -1.0f } );
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, 1.0f } );
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, 1.0f } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, 1.0f } );
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, 1.0f } );
        vertices.insert( vertices.end( ), { 1.0f, -1.0f, 1.0f } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, 1.0f } );

    }

    uint32_t getVertexCount( )
    {
        return 36;
    }

    [[nodiscard]] const std::vector< float > &getVertices( ) const
    {
        return vertices;
    }
};

class BuiltinPrimitives
{
public:
    static std::string getPrimitivePath( PrimitiveType type )
    {
        switch ( type )
        {
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
