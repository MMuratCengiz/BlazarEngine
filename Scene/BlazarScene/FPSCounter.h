#pragma once

#include <BlazarCore/Common.h>
#include <thread>

NAMESPACES( ENGINE_NAMESPACE, Scene )

class FPSCounter
{
private:
    uint32_t fpsCounter = 0;
    uint32_t start = 0;
    bool exitFlag = false;

    FPSCounter( )
    {
        start = nowInSeconds( );
    }

public:
    static FPSCounter Instance( )
    {
        static FPSCounter inst { };
        return inst;
    }

    static uint32_t nowInSeconds( )
    {
        return std::chrono::duration_cast< std::chrono::seconds >(
                std::chrono::system_clock::now( ).time_since_epoch( ) ).count( );
    }

    void tick( )
    {
        fpsCounter++;

        if ( nowInSeconds( ) - start > 1 )
        {
            start = nowInSeconds( );
            std::stringstream s;
            s << "FPS: " << fpsCounter;
            TRACE( "FPSCounter", VERBOSITY_CRITICAL, s.str( ).c_str( ) );

            fpsCounter = 0;
        }
    }
};

END_NAMESPACES
