/*
Blazar Engine - 3D Game Engine
Copyright (c) 2020-2021 Muhammed Murat Cengiz

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#define SHOULD_FLIP_Y false

#define FLIP_Y( data, start, offset ) for ( int i = start; i < data.size( ); i += offset ) data[ i ] *= -1;

#include <BlazarCore/Common.h>
#include <BlazarCore/DynamicMemory.h>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

enum class PrimitiveType : uint32_t
{
	LitCube = 0,
	PlainSquare = 1,
	TexturedSquare = 2,
	PlainTriangle = 3,
	PlainCube = 4
};

class IPrimitive {
public:
    virtual uint32_t getVertexCount( ) = 0;
    virtual const std::vector< float >& getData( ) const = 0;
    virtual std::string getPath( ) const = 0;
    virtual PrimitiveType getType( ) const = 0;
};

class LitCubePrimitive : public IPrimitive
{
	std::vector< float > data;
public:
	LitCubePrimitive( )
	{
		// Left Cube, Triangle 1
		data.insert( data.end( ), { -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, } );
		data.insert( data.end( ), { -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, } );
		data.insert( data.end( ), { -1.0f, 1.0f, 1.0f,   1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, } );
		// Left Cube, Triangle 2
		data.insert( data.end( ), { -1.0f, 1.0f, 1.0f,   1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, } );
		data.insert( data.end( ), { -1.0f, 1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, } );
		data.insert( data.end( ), { -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, } );
		////////////////////////////////////////////////////////////////////////////////////////////////
		// Back Cube, Triangle 1
		data.insert( data.end( ), { -1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, } );
		data.insert( data.end( ), { 1.0f, 1.0f, -1.0f,    1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, } );
		data.insert( data.end( ), { 1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, } );
		// Back Cube, Triangle 2
		data.insert( data.end( ), { -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, } );
		data.insert( data.end( ), { -1.0f, 1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, } );
		data.insert( data.end( ), { 1.0f, 1.0f, -1.0f,   1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, } );
		////////////////////////////////////////////////////////////////////////////////////////////////
		// Bottom Cube, Triangle 1
		data.insert( data.end( ), { -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, } );
		data.insert( data.end( ), { 1.0f, -1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, } );
		data.insert( data.end( ), { 1.0f, -1.0f, 1.0f,   1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, } );
		// Bottom Cube, Triangle 2
		data.insert( data.end( ), { -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, } );
		data.insert( data.end( ), { 1.0f, -1.0f, 1.0f,   1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, } );
		data.insert( data.end( ), { -1.0f, -1.0f, 1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, } );
		////////////////////////////////////////////////////////////////////////////////////////////////
		// Top Cube, Triangle 1
		data.insert( data.end( ), { -1.0f, 1.0f, -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, } );
		data.insert( data.end( ), { -1.0f, 1.0f, 1.0f,   1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, } );
		data.insert( data.end( ), { 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, } );
		// Top Cube, Triangle 2                                                  
		data.insert( data.end( ), { -1.0f, 1.0f, -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, } );
		data.insert( data.end( ), { 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, } );
		data.insert( data.end( ), { 1.0f, 1.0f, -1.0f,   1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, } );
		//////////////////////////////////////////////// ////////////////////////////////////////////////
		// Right Cube, Triangle 1                        
		data.insert( data.end( ), { 1.0f, 1.0f, -1.0f,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, } );
		data.insert( data.end( ), { 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, } );
		data.insert( data.end( ), { 1.0f, -1.0f, 1.0f,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, } );
		// Right Cube, Triangle 2                        
		data.insert( data.end( ), { 1.0f, -1.0f, 1.0f,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, } );
		data.insert( data.end( ), { 1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, } );
		data.insert( data.end( ), { 1.0f, 1.0f, -1.0f,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, } );
		//////////////////////////////////////////////// ////////////////////////////////////////////////
		// Front Cube, Triangle 1                        
		data.insert( data.end( ), { -1.0f, 1.0f, 1.0f,   1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, } );
		data.insert( data.end( ), { -1.0f, -1.0f, 1.0f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, } );
		data.insert( data.end( ), { 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, } );
		// Front Cube, Triangle 2                       
		data.insert( data.end( ), { -1.0f, -1.0f, 1.0f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, } );
		data.insert( data.end( ), { 1.0f, -1.0f, 1.0f,   1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, } );
		data.insert( data.end( ), { 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f } );

		if ( SHOULD_FLIP_Y )
		{
			FLIP_Y( data, 1, 8 )
		}
	}

	uint32_t getVertexCount( ) override
	{
		return 36;
	}

	[[nodiscard]] const std::vector< float >& getData( ) const override
	{
		return data;
	}

    [[nodiscard]] std::string getPath( ) const override
    {
        return "BuiltinPrimitives/LitCube";
    }

    [[nodiscard]] PrimitiveType getType( ) const override
    {
        return PrimitiveType::LitCube;
    }
};

class PlainCubePrimitive : public IPrimitive
{
	std::vector< float > data;
public:
	PlainCubePrimitive( )
	{
		data.insert( data.end( ), { -1.0f, -1.0f, -1.0f , 1.0f } );
		data.insert( data.end( ), { -1.0f, -1.0f, 1.0f  , 1.0f } );
		data.insert( data.end( ), { -1.0f, 1.0f, 1.0f   , 1.0f } );
		data.insert( data.end( ), { -1.0f, 1.0f, 1.0f   , 1.0f } );
		data.insert( data.end( ), { -1.0f, 1.0f, -1.0f  , 1.0f } );
		data.insert( data.end( ), { -1.0f, -1.0f, -1.0f , 1.0f } );
		data.insert( data.end( ), { -1.0f, -1.0f, -1.0f , 1.0f } );
		data.insert( data.end( ), { 1.0f, 1.0f, -1.0f   , 1.0f } );
		data.insert( data.end( ), { 1.0f, -1.0f, -1.0f  , 1.0f } );
		data.insert( data.end( ), { -1.0f, -1.0f, -1.0f , 1.0f } );
		data.insert( data.end( ), { -1.0f, 1.0f, -1.0f  , 1.0f } );
		data.insert( data.end( ), { 1.0f, 1.0f, -1.0f   , 1.0f } );
		data.insert( data.end( ), { -1.0f, -1.0f, -1.0f , 1.0f } );
		data.insert( data.end( ), { 1.0f, -1.0f, -1.0f  , 1.0f } );
		data.insert( data.end( ), { 1.0f, -1.0f, 1.0f   , 1.0f } );
		data.insert( data.end( ), { -1.0f, -1.0f, -1.0f , 1.0f } );
		data.insert( data.end( ), { 1.0f, -1.0f, 1.0f   , 1.0f } );
		data.insert( data.end( ), { -1.0f, -1.0f, 1.0f  , 1.0f } );
		data.insert( data.end( ), { -1.0f, 1.0f, -1.0f  , 1.0f } );
		data.insert( data.end( ), { -1.0f, 1.0f, 1.0f   , 1.0f } );
		data.insert( data.end( ), { 1.0f, 1.0f, 1.0f    , 1.0f } );
		data.insert( data.end( ), { -1.0f, 1.0f, -1.0f  , 1.0f } );
		data.insert( data.end( ), { 1.0f, 1.0f, 1.0f    , 1.0f } );
		data.insert( data.end( ), { 1.0f, 1.0f, -1.0f   , 1.0f } );
		data.insert( data.end( ), { 1.0f, 1.0f, -1.0f   , 1.0f } );
		data.insert( data.end( ), { 1.0f, 1.0f, 1.0f    , 1.0f } );
		data.insert( data.end( ), { 1.0f, -1.0f, 1.0f   , 1.0f } );
		data.insert( data.end( ), { 1.0f, -1.0f, 1.0f   , 1.0f } );
		data.insert( data.end( ), { 1.0f, -1.0f, -1.0f  , 1.0f } );
		data.insert( data.end( ), { 1.0f, 1.0f, -1.0f   , 1.0f } );
		data.insert( data.end( ), { -1.0f, 1.0f, 1.0f   , 1.0f } );
		data.insert( data.end( ), { -1.0f, -1.0f, 1.0f  , 1.0f } );
		data.insert( data.end( ), { 1.0f, 1.0f, 1.0f    , 1.0f } );
		data.insert( data.end( ), { -1.0f, -1.0f, 1.0f  , 1.0f } );
		data.insert( data.end( ), { 1.0f, -1.0f, 1.0f   , 1.0f } );
		data.insert( data.end( ), { 1.0f, 1.0f, 1.0f    , 1.0f } );

		if ( SHOULD_FLIP_Y )
		{
			FLIP_Y( data, 1, 8 )
		}
	}

	uint32_t getVertexCount( ) override
	{
		return 36;
	}

	[[nodiscard]] const std::vector< float >& getData( ) const override
	{
		return data;
	}

    [[nodiscard]] std::string getPath( ) const override
    {
        return "BuiltinPrimitives/PlainCube";
    }

    [[nodiscard]] PrimitiveType getType( ) const override
    {
        return PrimitiveType::PlainCube;
    }
};

class PlainSquarePrimitive : public IPrimitive
{
	std::vector< float > data;
public:
	PlainSquarePrimitive( )
	{
		data.insert( data.end( ), { -1.0f, -1.0f, 0.0f, 1.0f, } );
		data.insert( data.end( ), { -1.0f, 1.0f, 0.0f, 1.0f, } );
		data.insert( data.end( ), { 1.0f, -1.0f, 0.0f, 1.0f, } );
													 
		data.insert( data.end( ), { 1.0f, -1.0f, 0.0f, 1.0f, } );
		data.insert( data.end( ), { 1.0f, 1.0f, 0.0f, 1.0f, } );
		data.insert( data.end( ), { -1.0f, 1.0f, 0.0f, 1.0f, } );

		if ( SHOULD_FLIP_Y )
		{
			FLIP_Y( data, 1, 8 )
		}
	}

	uint32_t getVertexCount( ) override
	{
		return 6;
	}

	[[nodiscard]] const std::vector< float >& getData( ) const override
	{
		return data;
	}

    [[nodiscard]] std::string getPath( ) const override
    {
        return "BuiltinPrimitives/PlainSquare";
    }

    [[nodiscard]] PrimitiveType getType( ) const override
    {
        return PrimitiveType::PlainSquare;
    }
};

class TexturedSquarePrimitive : public IPrimitive
{
	std::vector< float > data;
public:
    TexturedSquarePrimitive( )
	{
		data.insert( data.end( ), { 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f } );
		data.insert( data.end( ), { 0.0f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f } );
		data.insert( data.end( ), { 1.0f, 0.0f,  0.0f, 1.0f, 1.0f, 0.0f } );

		data.insert( data.end( ), { 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f } );
		data.insert( data.end( ), { 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f } );
		data.insert( data.end( ), { 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f } );

		if ( SHOULD_FLIP_Y )
		{
			FLIP_Y( data, 1, 8 )
		}
	}

	uint32_t getVertexCount( ) override
	{
		return 6;
	}

	[[nodiscard]] const std::vector< float >& getData( ) const override
	{
		return data;
	}

    [[nodiscard]] std::string getPath( ) const override
    {
        return "BuiltinPrimitives/TexturedSquare";
    }

    [[nodiscard]] PrimitiveType getType( ) const override
    {
        return PrimitiveType::TexturedSquare;
    }
};

class PlainTrianglePrimitive : public IPrimitive
{
	std::vector< float > data;
public:
	PlainTrianglePrimitive( )
	{
		data.insert( data.end( ), { -1.0f, -1.0f, 0.0f, 1.0f } );
		data.insert( data.end( ), { -1.0f, 1.0f, 0.0f, 1.0f } );
		data.insert( data.end( ), { 1.0f, -1.0f, 0.0f, 1.0f } );

		if ( SHOULD_FLIP_Y )
		{
			FLIP_Y( data, 1, 8 )
		}
	}

	uint32_t getVertexCount( ) override
	{
		return 3;
	}

	[[nodiscard]] const std::vector< float >& getData( ) const override
	{
		return data;
	}

    [[nodiscard]] std::string getPath( ) const override
    {
        return "BuiltinPrimitives/PlainTriangle";
    }

    [[nodiscard]] PrimitiveType getType( ) const override
    {
        return PrimitiveType::PlainTriangle;
    }
};

class BuiltinPrimitives
{
public:
    // todo remove in favor of direct access
	static std::string getPrimitivePath( PrimitiveType type )
	{
		switch ( type )
		{
		case PrimitiveType::LitCube:
			return LitCubePrimitive{ }.getPath();
		case PrimitiveType::PlainCube:
            return PlainCubePrimitive{ }.getPath();
		case PrimitiveType::PlainSquare:
            return PlainSquarePrimitive{ }.getPath();
		case PrimitiveType::PlainTriangle:
            return PlainTrianglePrimitive{ }.getPath();
		case PrimitiveType::TexturedSquare:
            return TexturedSquarePrimitive{ }.getPath();
		}

		return "";
	}
};

END_NAMESPACES
