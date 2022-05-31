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

#include <BlazarInput/ActionMap.h>
#include <unordered_map>
#include <vector>

NAMESPACES( ENGINE_NAMESPACE, Input )

ActionMap::ActionMap( EventHandler *eventHandler ) : eventHandler( eventHandler )
{
    proxyActionCallback = [ & ]( const std::string &actionName, const KeyState &keyPressForm, const float &pressure )
    {
        FUNCTION_BREAK( callbacks.find( actionName ) == callbacks.end( ) );

        for ( auto &callback: callbacks[ actionName ] )
        {
            callback( actionName, keyPressForm, pressure );
        }
    };
}

void ActionMap::registerAction( const std::string &actionName, const std::initializer_list< KeyboardKeyCode > &codes )
{
    registerActionInternal( actionName, codes );
}

void ActionMap::registerAction( const std::string &actionName, const std::initializer_list< MouseKeyCode > &codes )
{
    registerActionInternal( actionName, codes );
}

void ActionMap::registerAction( const std::string &actionName, const int& gamepadIdx, const std::initializer_list< GamepadKeyCode > &codes )
{
    FUNCTION_BREAK( codes.size( ) == 0 )

    eventHandler->registerCallback( gamepadIdx, data( codes )[ 0 ], [ = ]( const KeyState &form, const GamepadKeyCode &code )
    {
        bool allSelected = true;
        for ( int i = 1; i < codes.size( ); ++i )
        {
            KeyState pressForm = eventHandler->checkKey( gamepadIdx, data( codes )[ i ] );
            allSelected = allSelected && ( pressForm == KeyState::Pressed || pressForm == KeyState::Repeated );
        }

        FUNCTION_BREAK( !allSelected );
        proxyActionCallback( actionName, form, 0.0f );
    } );
}

void ActionMap::subscribeToAction( const std::string &actionName, const ActionCallback &callback )
{
    if ( callbacks.find( actionName ) == callbacks.end( ) )
    {
        callbacks[ actionName ] = { };
    }

    callbacks[ actionName ].emplace_back( callback );
}

void ActionMap::registerScroll( const ScrollEventCallback & scrollEventCallback )
{
    eventHandler->registerScrollCallback( scrollEventCallback );
}

void ActionMap::registerGamepadAxisMove( const int& gamepadIdx, const GamepadAxisEventCallback & axisMoveCallback )
{
    eventHandler->registerCallback( gamepadIdx, axisMoveCallback );
}

void ActionMap::registerMouseMove( const MouseMoveEventCallback &mouseMoveCallback )
{
    eventHandler->registerMouseMoveCallback( mouseMoveCallback );
}

END_NAMESPACES
