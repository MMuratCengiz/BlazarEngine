//
// Created by Murat on 11/24/2020.
//

#include "ActionMap.h"

NAMESPACES( SomeVulkan, Input )

ActionMap::ActionMap( std::shared_ptr< EventHandler > eventHandler ) : eventHandler( std::move( eventHandler ) ) { }

void ActionMap::registerAction( const std::string &actionName, const ActionBinding &binding, const ActionCallback &callback ) {
    if ( callbacks.find( actionName ) == callbacks.end( ) ) {
        callbacks[ actionName ] = { };
    }

    callbacks[ actionName ].emplace_back( callback );

    if ( binding.controller == Controller::Keyboard ) {
        if ( binding.pressForm == KeyPressForm::Pressed ) {
            eventHandler->registerKeyboardPress( binding.keyCode, [ = ]( const KeyboardKeyCode &code ) {
                callback( actionName );
            } );
        }
    }

}

END_NAMESPACES
