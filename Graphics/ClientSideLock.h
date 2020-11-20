#pragma once

#include "../core/Common.h"

NAMESPACES( SomeVulkan, Graphics )

class ClientSideLock {
public:
    inline void acquire() {

    }

    inline void release() {

    }
};

typedef std::unique_ptr< ClientSideLock > upClientSideLock;

END_NAMESPACES