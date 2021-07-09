/*
Blazar Engine - 3D Game Engine
Copyright (c) 2020-2021 Muhammed Murat Cengiz

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
            Core::Logger::get( ).log( Core::Verbosity::Debug, "FPSCounter", s.str( ).c_str( ) );

            fpsCounter = 0;
        }
    }
};

END_NAMESPACES
