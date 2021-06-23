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

#define GLFW_INCLUDE_VULKAN
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
    void *userPointer { }; // todo maybe remove
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

enum class EventType
{
    WindowResized,
    SwapChainInvalidated,
    Tick
};

namespace FunctionDefinitions
{
typedef std::function< void( const EventType &type, std::shared_ptr< IEventParameters > eventParams ) > EventCallback;
}


class GlobalEventHandler
{
private:
    GlobalEventHandler( ) = default;
    std::unordered_map< EventType, std::vector< FunctionDefinitions::EventCallback > > eventSubscribers;
public:
    void initWindowEvents( GLFWwindow *window );
    void subscribeToEvent( const EventType &event, const FunctionDefinitions::EventCallback &cb );
    void triggerEvent( const EventType &event, const std::shared_ptr< IEventParameters > &parameters );

    static GlobalEventHandler &Instance( )
    {
        static GlobalEventHandler inst { };
        return inst;
    }

    static std::shared_ptr< TickParameters > createTickParameters( GLFWwindow *pWwindow );

    inline static std::shared_ptr< WindowResizedParameters > ToWindowResizedParameters( const std::shared_ptr< IEventParameters > &parameters )
    {
        return std::dynamic_pointer_cast< WindowResizedParameters >( parameters );
    }

    inline static std::shared_ptr< TickParameters > ToTickParameters( const std::shared_ptr< IEventParameters > &parameters )
    {
        return std::dynamic_pointer_cast< TickParameters >( parameters );
    }

    GlobalEventHandler( GlobalEventHandler const & ) = delete;
    void operator=( GlobalEventHandler const & ) = delete;
    void cleanup( );
private:
    void ensureMapContainsEvent( const EventType &event );
};

typedef std::shared_ptr< IEventParameters > pEventParameters;

END_NAMESPACES