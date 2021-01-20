#pragma once

#define SHOULD_FLIP_Y false

#define FLIP_Y( data, start, offset ) for ( int i = start; i < data.size( ); i += offset ) data[ i ] *= -1;

#include <BlazarCore/Common.h>
#include <BlazarCore/DynamicMemory.h>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

enum class PrimitiveType
{
    LightedCube,
    PlainSquare,
    PlainTriangle,
    PlainCube
};

struct PrimitiveData
{
    std::vector< float > vertices;
    uint32_t vertexCount;

    std::vector< float > indices;
    uint32_t indexCount;
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

        if ( SHOULD_FLIP_Y )
        {
            FLIP_Y( vertices, 1, 8 )
        }
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

        if ( SHOULD_FLIP_Y )
        {
            FLIP_Y( vertices, 1, 8 )
        }
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

class PlainSquarePrimitive
{
    std::vector< float > vertices;
public:
    PlainSquarePrimitive( )
    {
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, 0.0f } );
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, 0.0f } );
        vertices.insert( vertices.end( ), { 1.0f, -1.0f, 0.0f } );

        vertices.insert( vertices.end( ), { 1.0f, -1.0f, 0.0f } );
        vertices.insert( vertices.end( ), { 1.0f, 1.0f, 0.0f } );
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, 0.0f } );

        if ( SHOULD_FLIP_Y )
        {
            FLIP_Y( vertices, 1, 8 )
        }
    }

    uint32_t getVertexCount( )
    {
        return 6;
    }

    [[nodiscard]] const std::vector< float > &getVertices( ) const
    {
        return vertices;
    }
};

class PlainTrianglePrimitive
{
    std::vector< float > vertices;
public:
    PlainTrianglePrimitive( )
    {
        vertices.insert( vertices.end( ), { -1.0f, -1.0f, 0.0f } );
        vertices.insert( vertices.end( ), { -1.0f, 1.0f, 0.0f } );
        vertices.insert( vertices.end( ), { 1.0f, -1.0f, 0.0f } );

        if ( SHOULD_FLIP_Y )
        {
            FLIP_Y( vertices, 1, 8 )
        }
    }

    uint32_t getVertexCount( )
    {
        return 3;
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
            case PrimitiveType::PlainCube:
                return "BuiltinPrimitives/PlainCube";
            case PrimitiveType::PlainSquare:
                return "BuiltinPrimitives/PlainSquare";
            case PrimitiveType::PlainTriangle:
                return "BuiltinPrimitives/PlainTriangle";
        }

        return "";
    }
};

END_NAMESPACES
