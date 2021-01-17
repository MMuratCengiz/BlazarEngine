//
// Created by Murat on 10/29/2020.
//
#include "Time.h"

NAMESPACES( ENGINE_NAMESPACE, Core )

double Time::prev = 0;
double Time::deltaTime = 0.0f;

void Time::tick( )
{
    if ( prev == 0 )
    {
        prev = doubleEpochNow( );
        return;
    }

    double now = doubleEpochNow( );

    deltaTime = ( now - prev ) / 1000.0f; //std::max( now - prev, (double) 1 / 60.f );
    prev = now;
}

double Time::getDeltaTime( )
{
    return deltaTime;
}

double Time::doubleEpochNow( )
{
    return std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now( ).time_since_epoch( ) ).count( );
}

END_NAMESPACES