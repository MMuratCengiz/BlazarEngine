//
// Created by Murat on 10/21/2020.
//

#include "GlobalEventHandler.h"

NAMESPACES( SomeVulkan, Input )

void GlobalEventHandler::subscribeToEvent( const EventType& event, const FunctionDefinitions::EventCallback& cb ) {
    ensureMapContainsEvent( event );
    eventSubscribers[ event ].emplace_back( cb );
}

void GlobalEventHandler::triggerEvent( const EventType& event, const std::shared_ptr< IEventParameters >& parameters ) {
    ensureMapContainsEvent( event );

    for ( const auto& cb: eventSubscribers[ event ] ) {
        cb( event, parameters );
    }
}

void GlobalEventHandler::ensureMapContainsEvent( const EventType& event ) {
    if ( eventSubscribers.find( event ) == eventSubscribers.end( ) ) {
        eventSubscribers[ event ] = { };
    }
}

void GlobalEventHandler::initWindowEvents( GLFWwindow *window ) {
    glfwSetWindowUserPointer( window, ( void * ) this );

    glfwSetFramebufferSizeCallback( window, [ ]( GLFWwindow *window, int width, int height ) {
        void * userPointer  = glfwGetWindowUserPointer( window );
        auto * eventHandler = static_cast< GlobalEventHandler * >( userPointer );

        auto parameters = std::make_shared< WindowResizedParameters >( );
        parameters->width = width;
        parameters->height = height;
        eventHandler->triggerEvent( EventType::WindowResized, parameters );
        eventHandler->triggerEvent( EventType::SwapChainInvalidated, nullptr );
    } );

}

END_NAMESPACES