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

#include <utility>
#include "Common.h"

NAMESPACES( ENGINE_NAMESPACE, Core )

class DynamicMemory
{
private:
    char *buffer;
    uint64_t currentSize;
    uint64_t bufferSize;
public:
    inline explicit DynamicMemory( uint64_t initialSize = 1024 ) : currentSize( 0 ), bufferSize( initialSize )
    {
        buffer = static_cast< char * >( malloc( initialSize ) );
    }

    void setInitialSize( uint64_t size )
    {
        free( buffer );
        buffer = static_cast< char * >( malloc( size ) );
        bufferSize = size;
    }

    template< typename T >
    void attachElement( T e )
    {
        uint32_t size = sizeof( T );

        expandIfRequired( size );

        memcpy( buffer + currentSize, &e, size );
        currentSize += size;
    }


    template< typename T >
    void attachElements( std::vector< T > elements )
    {
        uint32_t additionalSize = elements.size( ) * sizeof( T );

        expandIfRequired( additionalSize );

        memcpy( buffer + currentSize, elements.data( ), additionalSize );
        currentSize += additionalSize;
    }

    const void *data( uint64_t offset = 0 ) const
    {
        float *f = ( float * ) ( void * ) buffer;
        return static_cast< const void * >( buffer + offset );
    }

    [[nodiscard]] const uint64_t &size( ) const
    {
        return currentSize;
    }

    ~DynamicMemory( )
    {
        free( buffer );
    }

private:
    void expandIfRequired( const uint32_t &additionalSize )
    {
        if ( additionalSize + currentSize > bufferSize )
        {
            bufferSize += std::max< uint32_t >( 1024, additionalSize );

            buffer = static_cast< char * >( realloc( buffer, bufferSize ) );
        }
    }
};

END_NAMESPACES