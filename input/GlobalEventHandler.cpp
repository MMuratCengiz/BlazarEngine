//
// Created by Murat on 10/21/2020.
//

#include "GlobalEventHandler.h"
#include "../core/Game.h"

using namespace SomeVulkan::Input;

void SomeVulkan::Input::GlobalEventHandler::addWindowResizeCallback( GLFWwindow * window, void * userPointer, const TFUNC::WindowResize& cb ) {
    glfwSetWindowUserPointer( window, ( void * ) this );

    glfwSetFramebufferSizeCallback( window, [ ]( GLFWwindow *window, int width, int height ) {
        void * userPointer  = glfwGetWindowUserPointer( window );
        auto * eventHandler = static_cast< GlobalEventHandler * >( userPointer );

        for ( uint32_t pairIndex: eventHandler->windowResizeCallbackIndexes ) {
            auto functor = eventHandler->windowResizeFunctors[ pairIndex ];
            void * pUser = eventHandler->windowResizeUserPointers [ pairIndex ];

            functor( pUser, width, height );
        }
    } );

    unsigned int index = windowResizeCallbackIndexes.size( ) + 1;
    windowResizeCallbackIndexes.emplace_back( index );
    windowResizeUserPointers[ index ] = userPointer;
    windowResizeFunctors[ index ] = cb;
}
