#pragma once

#include "../Core/Common.h"

NAMESPACES( SomeVulkan, Scene )

class FPSCounter {
private:
    uint32_t fpsCounter = 0;
    uint32_t start = 0;
    FPSCounter() {
        start = nowInSeconds( );
    }
public:
    static FPSCounter Instance( ) {
        static FPSCounter inst{ };
        return inst;
    }

    static uint32_t nowInSeconds( ) {
        return std::chrono::duration_cast< std::chrono::seconds >(
                std::chrono::system_clock::now( ).time_since_epoch( ) ).count( );
    }

    void tick() {
        fpsCounter++;

        if ( nowInSeconds( ) - start > 1 ) {
            start = nowInSeconds( );
            std::stringstream s;
            s << "FPS: " << fpsCounter;
            TRACE( COMPONENT_GAMEH, VERBOSITY_CRITICAL, s.str( ).c_str( ) );

            fpsCounter = 0;
        }
    }
};

END_NAMESPACES
