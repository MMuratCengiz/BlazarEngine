//
// Created by Murat on 11/24/2020.
//

#include "EventHandler.h"
#include <unordered_map>
#include <vector>

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
