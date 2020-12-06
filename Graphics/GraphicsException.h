#pragma once

#include "../Core/Common.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class GraphicsException : public std::exception {
public:
    enum class Source: uint32_t {
        RenderDevice = 0,
        RenderSurface = 1,
        Renderer = 2,
        Utilities = 3,
    };
private:

    std::string formattedError;

    static std::string sourceToStr( Source source ) {
        std::string sourceList[] {
                "RenderDevice",
                "RenderSurface",
                "Renderer",
                "Utilities" };

        std::string result = sourceList[ uint32_t( source ) ];

        return "[" + result + "]";
    }

public:
    GraphicsException( Source source, const std::string &message ) {
        SFORMAT( 1, sourceToStr( source ) << ": " << message, formattedError );
    }

    [[nodiscard]] const char *what( ) const noexcept override {
        return formattedError.data( );
    }
};

END_NAMESPACES
