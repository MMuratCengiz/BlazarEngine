#pragma once

#include "../core/Common.h"

NAMESPACES( SomeVulkan, Graphics )

class GraphicsException : public std::exception {
public:
    enum class Source: uint32_t {
        RenderDevice = 0
    };
private:
    std::string formattedError;

    static std::string sourceToStr( Source source ) {
        std::string result = ( ( std::string[] ) { "RenderDevice" } )[ ( uint32_t ) source ];

        return "[" + result + "]";
    }

public:
    GraphicsException( Source source, const std::string &message ) {
        SFORMAT( 1, sourceToStr( source ) << ":" << message, formattedError );
    }

    [[nodiscard]] const char *what( ) const noexcept override {
        return formattedError.data( );
    }
};

END_NAMESPACES
