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

#include <BlazarInput/EventHandler.h>

NAMESPACES( ENGINE_NAMESPACE, Input )

EventHandler::EventHandler( GLFWwindow *window ) : window( window )
{
    glfwSetWindowUserPointer( window, this );

    glfwSetKeyCallback( window, [ ]( GLFWwindow *window, int key, int scancode, int action, int mods ) {
        auto * handler = ( EventHandler * ) glfwGetWindowUserPointer( window );

        auto it = handler->keyboardCallbacks.find( ( KeyboardKeyCode ) key );
        if ( it != handler->keyboardCallbacks.end( ) )
        {
            for ( const auto &callback: it->second )
            {
                callback( ( KeyState ) action, ( KeyboardKeyCode ) key );
            }
        }
    });

    glfwSetMouseButtonCallback( window, [ ]( GLFWwindow *window, int button, int action, int mods ) {
        auto * handler = ( EventHandler * ) glfwGetWindowUserPointer( window );

        auto it = handler->mouseKeyCallbacks.find( ( MouseKeyCode ) button );
        if ( it != handler->mouseKeyCallbacks.end( ) )
        {
            for ( const auto &callback: it->second )
            {
                callback( ( KeyState ) action, ( MouseKeyCode ) button );
            }
        }
    });

    glfwSetCursorPosCallback( window, []( GLFWwindow * window, double x, double y ) {
        auto * handler = ( EventHandler * ) glfwGetWindowUserPointer( window );
        for ( auto & cb: handler->mouseMoveCallbacks ) {
            cb( x, y );
        }
    } );

    glfwSetScrollCallback( window, []( GLFWwindow * window, double x, double y ) {
        auto * handler = ( EventHandler * ) glfwGetWindowUserPointer( window );
        for ( auto & cb: handler->scrollCallbacks ) {
            cb( x, y );
        }
    } );
}

void EventHandler::registerCallback( const KeyboardKeyCode &code, const KeyboardEventCallback &callback )
{
    registerEvent( code, keyboardCallbacks, callback);
}

void EventHandler::registerCallback( const MouseKeyCode &code, const MouseEventCallback &callback )
{
    registerEvent( code, mouseKeyCallbacks, callback);
}

void EventHandler::registerCallback( const int& gamepadIdx, const GamepadKeyCode &code, const GamepadKeyEventCallback &callback )
{
    if ( gamepadKeyCallbacks.size( ) <= gamepadIdx )
    {
        gamepadKeyCallbacks.resize( gamepadIdx );
    }

    registerEvent( code, gamepadKeyCallbacks[ gamepadIdx ], callback);
}

void EventHandler::registerCallback( const int& gamepadIdx, const GamepadAxisEventCallback &callback )
{
    if ( gamepadAxisCallbacks.size( ) <= gamepadIdx )
    {
        gamepadAxisCallbacks.resize( gamepadIdx );
    }

    gamepadAxisCallbacks[ gamepadIdx ].push_back( callback );
}

void EventHandler::registerMouseMoveCallback( const MouseMoveEventCallback &callback )
{
    mouseMoveCallbacks.push_back( callback );
}

void EventHandler::registerScrollCallback( const ScrollEventCallback &callback )
{
    scrollCallbacks.push_back( callback );
}

void EventHandler::pollEvents( )
{
    glfwPollEvents( );

    int maxSize = std::max( gamepadAxisCallbacks.size( ), gamepadKeyCallbacks.size( ) );

    if ( currentGamepadStates.size( ) < maxSize )
    {
        currentGamepadStates.resize( maxSize );
    }

    for ( int i = 0; i < maxSize; ++i )
    {
        bool gamepadConnected = glfwGetGamepadState( i, &currentGamepadStates[ i ] );

        if ( !gamepadConnected )
        {
            continue;
        }

        auto & gamepadState = currentGamepadStates[ i ];
        GamepadAxisPressure axisPressures { gamepadState.axes };

        for ( auto & axisCb: gamepadAxisCallbacks[ i ] )
        {
            axisCb( axisPressures );
        }

        for ( auto & cbMap: gamepadKeyCallbacks[ i ] )
        {
            auto state = gamepadState.buttons[ ( int ) cbMap.first ];
            if ( state == GLFW_PRESS )
            {
                for ( auto & cb: cbMap.second )
                {
                    cb( KeyState::Pressed, cbMap.first );
                }
            }
        }

    }


}

END_NAMESPACES
