#pragma once

#include "../Core/Common.h"

NAMESPACES( SomeVulkan, Scene )

class World {
private:
    World() {

    }

public:
    static std::shared_ptr< World > Instance( ) {
        auto * world = new World{ };
        static std::shared_ptr< World > inst = std::shared_ptr< World >( world );
        return inst;
    }
};

END_NAMESPACES