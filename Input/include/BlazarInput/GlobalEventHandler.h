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

#ifdef __APPLE_CC__
#else
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <unordered_map>
NAMESPACES( ENGINE_NAMESPACE, Input )

struct IEventParameters
{
    virtual ~IEventParameters( ) = default;
};

struct WindowResizedParameters : IEventParameters
{
    uint32_t width;
    uint32_t height;
};

struct TickParameters : IEventParameters
{
    GLFWwindow *window;
};

struct SwapChainInvalidatedParameters : IEventParameters
{
};

enum class EventType
{
    WindowResized,
    SwapChainInvalidated,
    Tick
};

template< class T >
class GlobalEventHandler
{
private:
    GlobalEventHandler( ) = default;
    std::unordered_map< EventType, std::vector<  std::function< void( T eventParams ) > > > eventSubscribers;
public:
    void subscribeToEvent( const EventType &event, const std::function< void( T eventParams ) > &cb ) {
        ensureMapContainsEvent( event );
        eventSubscribers[ event ].emplace_back( cb );
    }

    void triggerEvent( const EventType &event, T parameters ) {
        ensureMapContainsEvent( event );

        for ( const auto &cb: eventSubscribers[ event ] )
        {
            cb( parameters );
        }
    }

    static GlobalEventHandler &Instance( )
    {
        static GlobalEventHandler inst { };
        return inst;
    }


    GlobalEventHandler( GlobalEventHandler const & ) = delete;
    void operator=( GlobalEventHandler const & ) = delete;
    void cleanup( )
    {
        eventSubscribers.clear( );
    }
private:
    void ensureMapContainsEvent( const EventType &event )
    {
        if ( eventSubscribers.find( event ) == eventSubscribers.end( ) )
        {
            eventSubscribers[ event ] = { };
        }
    }
};

class Events {
public:
    template< typename T >
    static void subscribe( const EventType &event, const std::function< void( T eventParams ) > &cb ) {
        GlobalEventHandler<T>::Instance().subscribeToEvent(event, cb);
    }

    template< typename T >
    static void trigger( const EventType &event, T parameters ) {
        GlobalEventHandler<T>::Instance().triggerEvent(event, parameters);
    }

    static void initWindowEvents( GLFWwindow *window )
    {
        glfwSetFramebufferSizeCallback( window, [ ]( GLFWwindow *window, int width, int height )
        {
            auto windowResizedParameters = std::make_unique< WindowResizedParameters >( );
            auto swapChainInvalidatedParameters = std::make_unique< SwapChainInvalidatedParameters >( );
            windowResizedParameters->width = width;
            windowResizedParameters->height = height;

            GlobalEventHandler<WindowResizedParameters * >::Instance().triggerEvent( EventType::WindowResized, windowResizedParameters.get( ) );
            GlobalEventHandler<SwapChainInvalidatedParameters * >::Instance().triggerEvent(EventType::SwapChainInvalidated, swapChainInvalidatedParameters.get( ) );
        } );
    }
};

END_NAMESPACES