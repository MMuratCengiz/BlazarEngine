#pragma once

#include "../core/Common.h"

NAMESPACES( SomeVulkan, Input )

namespace TFUNC {
typedef std::function< void( void *userPointer, int newWidth, int newHeight ) > WindowResize;
}

class GlobalEventHandler {
private:
    std::vector< uint32_t > windowResizeCallbackIndexes { };
    std::unordered_map< uint32_t, void * > windowResizeUserPointers { };
    std::unordered_map< uint32_t, TFUNC::WindowResize > windowResizeFunctors{ };

    GlobalEventHandler( ) = default;
public:
    void addWindowResizeCallback( GLFWwindow *window, void *userPointer, const TFUNC::WindowResize& );

    static GlobalEventHandler &Instance( ) {
        static GlobalEventHandler inst { };
        return inst;
    }

    GlobalEventHandler( GlobalEventHandler const & ) = delete;
    void operator=( GlobalEventHandler const & ) = delete;
};

END_NAMESPACES