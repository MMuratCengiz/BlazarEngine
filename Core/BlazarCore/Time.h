#pragma once

#include "Common.h"
#include <chrono>

NAMESPACES( ENGINE_NAMESPACE, Core )

class Time
{
private:
    static double prev;
    static double deltaTime;
public:
    static double doubleEpochNow( );
    static void tick( );
    static double getDeltaTime( );
};


END_NAMESPACES