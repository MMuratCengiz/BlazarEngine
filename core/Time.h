#pragma once

#include "../core/Common.h"
#include <chrono>

NAMESPACES( SomeVulkan, Core )

class Time {
private:
    static long long int prev;
    static float deltaTime;
public:
    static void tick();
    static float getDeltaTime();
};



END_NAMESPACES