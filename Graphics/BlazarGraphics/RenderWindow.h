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
#include <BlazarInput/GlobalEventHandler.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <GLFW/glfw3.h>
#endif

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class RenderWindow
{
private:
#ifdef WIN32
    HINSTANCE hInstance;
    HWND hWindow;
#else
    GLFWwindow * window;
#endif
    uint32_t width;
    uint32_t height;

    std::vector< std::string > extensions;
public:
    RenderWindow( const uint32_t& width, const uint32_t& height, std::vector< std::string > extensions ) : width( width ), height( height ), extensions( std::move( extensions ) )
    {
        Input::Events::subscribe< Input::WindowResizedParameters * >(
                Input::EventType::WindowResized, [ & ](  Input::WindowResizedParameters *  windowResizeParameters )
                {
                    if ( windowResizeParameters->width > 0 && windowResizeParameters->height > 0 )
                    {
                        this->width = windowResizeParameters->width;
                        this->height = windowResizeParameters->height;
                    }
                });
    }

    inline const uint32_t& getWidth( ) const
    {
        return width;
    }

    inline const uint32_t& getHeight( ) const
    {
        return height;
    }

    inline const std::vector< std::string >& getExtensions( ) const
    {
        return extensions;
    }

#ifdef WIN32
    void setPlatformSpecific( HINSTANCE instance, HWND window )
    {
        this->hInstance = instance;
        this->hWindow = window;
    }

    const HINSTANCE getHInstance( ) const
    {
        return hInstance;
    }

    const HWND getHWindow( ) const
    {
        return hWindow;
    }
#else
    inline void setPlatformSpecific( GLFWwindow * window )
    {
        this->window = window;
    }

    inline GLFWwindow * getWindow()
    {
        return window;
    }
#endif
};

END_NAMESPACES

