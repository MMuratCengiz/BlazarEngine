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

#include "EventHandler.h"
#include <unordered_map>
#include <vector>
#include <execution>

NAMESPACES( ENGINE_NAMESPACE, Input )

void EventHandler::registerKeyboardPress( const KeyboardKeyCode &code, const KeyboardPressEventCallback &callback )
{
    if ( keyboardPressCallbacks.find( code ) == keyboardPressCallbacks.end( ) )
    {
        keyboardPressCallbacks[ code ] = { };
    }

    keyboardPressCallbacks[ code ].emplace_back( callback );
}

void EventHandler::pollEvents( )
{
    KeyboardKeyCode iter;

    uint32_t i = 0;

    do
    {
        iter = static_cast< KeyboardKeyCode >( i );

        if ( glfwGetKey( window, GLFW_KEY_A + i ) == GLFW_PRESS )
        {
            for ( const auto &callback: keyboardPressCallbacks[ iter ] )
            {
                callback( iter );
            }
        }

        ++i;
    } while ( iter != KeyboardKeyCode::Z );
}

END_NAMESPACES
