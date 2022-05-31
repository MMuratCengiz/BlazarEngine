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
#include "EventHandler.h"

NAMESPACES( ENGINE_NAMESPACE, Input )

typedef std::function< void( const std::string &, const KeyState &keyPressForm, const float &pressure ) > ActionCallback;

class ActionMap
{
private:
    EventHandler *eventHandler;
    std::unordered_map< std::string, std::vector< ActionCallback > > callbacks;
    ActionCallback proxyActionCallback;
public:
    explicit ActionMap( EventHandler *eventHandler );

    void registerAction( const std::string &actionName, const std::initializer_list< KeyboardKeyCode > &codes );

    void registerAction( const std::string &actionName, const std::initializer_list< MouseKeyCode > &codes );

    void registerAction( const std::string &actionName, const int &gamepadIdx, const std::initializer_list< GamepadKeyCode > &codes );

    void registerGamepadAxisMove( const int &gamepadIdx, const GamepadAxisEventCallback &axisMoveCallback );

    void registerScroll( const ScrollEventCallback &scrollEventCallback );

    void registerMouseMove( const MouseMoveEventCallback &moveMoveCallback );

    void subscribeToAction( const std::string &actionName, const ActionCallback &callback );

private:
    template< class tKeyCode >
    inline void registerActionInternal( const std::string &actionName, const std::initializer_list< tKeyCode > &codes )
    {
        FUNCTION_BREAK( codes.size( ) == 0 )

        eventHandler->registerCallback( data( codes )[ 0 ], [ = ]( const KeyState &form, const tKeyCode &code )
        {
            bool allSelected = true;
            for ( int i = 1; i < codes.size( ); ++i )
            {
                KeyState pressForm = eventHandler->checkKey( data( codes )[ i ] );
                allSelected = allSelected && ( pressForm == KeyState::Pressed || pressForm == KeyState::Repeated );
            }

            FUNCTION_BREAK( !allSelected );
            proxyActionCallback( actionName, form, 0.0f );
        } );
    }
};

END_NAMESPACES
