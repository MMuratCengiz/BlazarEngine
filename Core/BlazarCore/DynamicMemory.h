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