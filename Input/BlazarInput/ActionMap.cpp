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
