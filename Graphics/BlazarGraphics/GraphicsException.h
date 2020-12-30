#pragma once

#include <BlazarCore/Common.h>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class GraphicsException : public std::exception
{
private:
    std::string error;
public:
    GraphicsException( const std::string& source, const std::string &message )
    {
        std::stringstream formatter;
        formatter << "[" << source << "]" << ": " << message;
        error = formatter.str();
    }

    [[nodiscard]] const char *what( ) const noexcept override
    {
        return error.data( );
    }
};

END_NAMESPACES
