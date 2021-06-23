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
// Created by Murat on 10/21/2020.
//

#include "GlobalEventHandler.h"
#include <vector>

NAMESPACES( ENGINE_NAMESPACE, Input )

void GlobalEventHandler::subscribeToEvent( const EventType &event, const FunctionDefinitions::EventCallback &cb )
{
    ensureMapContainsEvent( event );
    eventSubscribers[ event ].emplace_back( cb );
}

void GlobalEventHandler::triggerEvent( const EventType &event, const std::shared_ptr< IEventParameters > &parameters )
{
    ensureMapContainsEvent( event );

    for ( const auto &cb: eventSubscribers[ event ] )
    {
        cb( event, parameters );
    }
}

void GlobalEventHandler::ensureMapContainsEvent( const EventType &event )
{
    if ( eventSubscribers.find( event ) == eventSubscribers.end( ) )
    {
        eventSubscribers[ event ] = { };
    }
}

void GlobalEventHandler::initWindowEvents( GLFWwindow *window )
{
    glfwSetWindowUserPointer( window, ( void * ) this );

    glfwSetFramebufferSizeCallback( window, [ ]( GLFWwindow *window, int width, int height )
    {
        void *userPointer = glfwGetWindowUserPointer( window );
        auto *eventHandler = static_cast< GlobalEventHandler * >( userPointer );

        auto parameters = std::make_shared< WindowResizedParameters >( );
        parameters->width = width;
        parameters->height = height;
        eventHandler->triggerEvent( EventType::WindowResized, parameters );
        eventHandler->triggerEvent( EventType::SwapChainInvalidated, nullptr );
    } );

}

std::shared_ptr< TickParameters > GlobalEventHandler::createTickParameters( GLFWwindow *window )
{
    auto params = std::make_shared< TickParameters >( );
    params->window = window;
    return params;
}

void GlobalEventHandler::cleanup( )
{
    eventSubscribers.clear( );
}

END_NAMESPACES