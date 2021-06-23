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