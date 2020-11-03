#pragma once


#include <utility>

#include "Common.h"

NAMESPACES( SomeVulkan, Core )

class DynamicMemory {
private:
    char *buffer;
    uint32_t currentSize;
    uint32_t bufferSize;
public:
    inline explicit DynamicMemory( uint32_t initialSize = 1024 ) : currentSize( 0 ), bufferSize( initialSize ) {
        buffer = static_cast< char * >( malloc( initialSize ) );
    }

    template< typename T >
    void attachElement( T e ) {
        uint32_t size = sizeof( e );

        expandIfRequired( size );

        memcpy( buffer + currentSize, &e, size );
        currentSize += size;
    }


    template< typename T >
    void attachElements( std::initializer_list< T > elements ) {
        for ( T e: elements ) {
            attachElement( e );
        }
    }

    const void *data( uint32_t offset = 0 ) const {
        return static_cast< const void * >( buffer + offset );
    }

    [[nodiscard]] const uint32_t &size( ) const {
        return currentSize;
    }

    ~DynamicMemory( ) {
        free( buffer );
    }

private:
    void expandIfRequired( const uint32_t& additionalSize ) {
        if ( additionalSize + currentSize > bufferSize ) {
            uint32_t newSize = bufferSize + std::max< uint32_t >( 1024, additionalSize );

            buffer = static_cast< char * >( realloc( static_cast< void * >( buffer ), newSize ) );
        }
    }
};

END_NAMESPACES