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

enum class KeyPressForm
{
    Pressed,
    Released,
    Holding
};

enum class KeyboardKeyCode : uint32_t
{
    A = 0,
    B = 1,
    C = 2,
    D = 3,
    E = 4,
    F = 5,
    G = 6,
    H = 7,
    I = 8,
    J = 9,
    K = 10,
    L = 11,
    M = 12,
    N = 13,
    O = 14,
    P = 15,
    Q = 16,
    R = 17,
    S = 18,
    T = 19,
    U = 20,
    V = 21,
    W = 22,
    X = 23,
    Y = 24,
    Z = 25
};

enum class MouseKeyCode
{
    LeftMouseClick,
    RightMouseClick
};

typedef std::function< void( KeyboardKeyCode ) > KeyboardPressEventCallback;

class EventHandler
{
private:
    GLFWwindow *window;
    std::unordered_map< KeyboardKeyCode, std::vector< KeyboardPressEventCallback > > keyboardPressCallbacks;
public:
    inline explicit EventHandler( GLFWwindow *window ) : window( window )
    { }

    void registerKeyboardPress( const KeyboardKeyCode &code, const KeyboardPressEventCallback &callback );
    void pollEvents( );
};

END_NAMESPACES

