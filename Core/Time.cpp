//
// Created by Murat on 10/29/2020.
//
#include "Time.h"

NAMESPACES( ENGINE_NAMESPACE, Core )

long long int Time::prev = 0;
float Time::deltaTime = 0.0f;

void Time::tick( ) {
    if ( prev == 0 ) {
        prev = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000;
        return;
    }

    auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000;

    deltaTime = 1.0f / ( now - prev ) * 60;
    prev = now;
}

float Time::getDeltaTime( ) {
    return deltaTime;
}

END_NAMESPACES