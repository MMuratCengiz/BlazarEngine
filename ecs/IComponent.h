#pragma once

#include "../core/Common.h"

NAMESPACES( SomeVulkan, ECS )

class IComponent {
public:
    virtual uint32_t getId() = 0;

    bool operator==( IComponent* com ) {
        return com->getId() == this->getId();
    }
};

END_NAMESPACES