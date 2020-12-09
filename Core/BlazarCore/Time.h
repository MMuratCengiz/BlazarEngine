#pragma once

#include "Common.h"
#include <chrono>

NAMESPACES( ENGINE_NAMESPACE, Core )

class Time
{
private:
    static long long int prev;
    static float deltaTime;
public:
    static void tick( );
    static float getDeltaTime( );
};


END_NAMESPACES