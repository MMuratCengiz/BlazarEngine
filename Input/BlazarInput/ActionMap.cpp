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
// Created by Murat on 11/24/2020.
//

#include "ActionMap.h"
#include <unordered_map>
#include <vector>

NAMESPACES( ENGINE_NAMESPACE, Input )

ActionMap::ActionMap( EventHandler* eventHandler ) : eventHandler( eventHandler )
{
    proxyActionCallback = [ & ]( const std::string &actionName )
    {
        FUNCTION_BREAK( callbacks.find( actionName ) == callbacks.end( ) );

        for ( auto& callback: callbacks[ actionName ] )
        {
            callback( actionName );
        }
    };
}

void ActionMap::registerAction( const std::string &actionName, ActionBinding binding)
{
    if ( binding.controller == Controller::Keyboard )
    {
        if ( binding.pressForm == KeyPressForm::Pressed )
        {
            eventHandler->registerKeyboardPress( binding.keyCode, [ = ]( const KeyboardKeyCode &code )
            {
                proxyActionCallback( actionName );
            } );
        }
    }
}

void ActionMap::subscribeToAction( const std::string &actionName, ActionCallback callback )
{
    if ( callbacks.find( actionName ) == callbacks.end( ) )
    {
        callbacks[ actionName ] = { };
    }

    callbacks[ actionName ].emplace_back( callback );
}

END_NAMESPACES
