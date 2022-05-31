// Blazar Engine - 3D Game Engine
// Copyright (c) 2020-2021 Muhammed Murat Cengiz
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

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

enum class KeyState : uint32_t
{
    Released = 0,
    Pressed = 1,
    Repeated = 2,
    None = 3
};

enum class KeyboardKeyCode : uint32_t
{
    Space = 32,
    Apostrophe = 39,
    Comma = 44,
    Minus = 45,
    Period = 46,
    Slash = 47,
    N0 = 48,
    N1 = 49,
    N2 = 50,
    N3 = 51,
    N4 = 52,
    N5 = 53,
    N6 = 54,
    N7 = 55,
    N8 = 56,
    N9 = 57,
    Semicolon = 59,
    Equal = 61,
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    LeftBracket = 91,
    Backslash = 92,
    RightBracket = 93,
    GraveAccent = 96,
    World_1 = 161,
    World_2 = 162,
    Escape = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,
    Insert = 260,
    Del = 261,
    Right = 262,
    Left = 263,
    Down = 264,
    Up = 265,
    PageUp = 266,
    PageDown = 267,
    Home = 268,
    End = 269,
    CapsLock = 280,
    ScrollLock = 281,
    NumLock = 282,
    PrintScreen = 283,
    Pause = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    F13 = 302,
    F14 = 303,
    F15 = 304,
    F16 = 305,
    F17 = 306,
    F18 = 307,
    F19 = 308,
    F20 = 309,
    F21 = 310,
    F22 = 311,
    F23 = 312,
    F24 = 313,
    F25 = 314,
    Kp0 = 320,
    Kp1 = 321,
    Kp2 = 322,
    Kp3 = 323,
    Kp4 = 324,
    Kp5 = 325,
    Kp6 = 326,
    Kp7 = 327,
    Kp8 = 328,
    Kp9 = 329,
    KpDecimal = 330,
    KpDivide = 331,
    KpMultiply = 332,
    KpSubtract = 333,
    KpAdd = 334,
    KpEnter = 335,
    KpEqual = 336,
    LeftShift = 340,
    LeftControl = 341,
    LeftAlt = 342,
    LeftSuper = 343,
    RightShift = 344,
    RightControl = 345,
    RightAlt = 346,
    RightSuper = 347,
    Menu = 348
};

enum class MouseKeyCode : uint32_t
{
    Button1 = 0,
    Button2 = 1,
    Button3 = 2,
    Button4 = 3,
    Button5 = 4,
    Button6 = 5,
    Button7 = 6,
    Button8 = 7
};

enum class GamepadKeyCode : uint32_t
{
    A = 0,
    B = 1,
    X = 2,
    Y = 3,
    LeftBumper = 4,
    RightBumper = 5,
    Back = 6,
    Start = 7,
    Guide = 8,
    LeftThumb = 9,
    RightThumb = 10,
    DpadUp = 11,
    DpadRight = 12,
    DpadDown = 13,
    DpadLeft = 14
};

struct GamepadAxisPressure
{
    float leftX;
    float leftY;
    float rightX;
    float rightY;
    float leftTrigger;
    float rightTrigger;

    explicit GamepadAxisPressure( float pressures[6] )
    {
        leftX = pressures[ 0 ];
        leftY = pressures[ 1 ];
        rightX = pressures[ 2 ];
        rightY = pressures[ 3 ];
        leftTrigger = pressures[ 4 ];
        rightTrigger = pressures[ 5 ];
    }
};

enum class ControllerCode : uint32_t
{
    Keyboard = 0,
    Mouse = 1,
    Joystick = 2,
    Gamepad = 3,
    GamepadAxis = 4,
    MouseMove = 5
};

typedef std::function< void( KeyboardKeyCode ) > KeyboardPressEventCallback;

typedef std::function< void( KeyState, KeyboardKeyCode ) > KeyboardEventCallback;
typedef std::function< void( KeyState, MouseKeyCode ) > MouseEventCallback;
typedef std::function< void( KeyState, GamepadKeyCode ) > GamepadKeyEventCallback;
typedef std::function< void( GamepadAxisPressure ) > GamepadAxisEventCallback;
typedef std::function< void( double x, double y ) > MouseMoveEventCallback;
typedef std::function< void( double x, double y ) > ScrollEventCallback;

class EventHandler
{
private:
    GLFWwindow *window;
    std::unordered_map< KeyboardKeyCode, std::vector< KeyboardPressEventCallback > > keyboardPressCallbacks;

    std::unordered_map< KeyboardKeyCode, std::vector< KeyboardEventCallback > > keyboardCallbacks;
    std::unordered_map< MouseKeyCode, std::vector< MouseEventCallback > > mouseKeyCallbacks;
    std::vector< std::unordered_map< GamepadKeyCode, std::vector< GamepadKeyEventCallback > > > gamepadKeyCallbacks;
    std::vector< std::vector< GamepadAxisEventCallback > > gamepadAxisCallbacks;

    std::vector< MouseMoveEventCallback > mouseMoveCallbacks;
    std::vector< ScrollEventCallback > scrollCallbacks;
    std::vector< GLFWgamepadstate > currentGamepadStates;
public:
    explicit EventHandler( GLFWwindow *window );

    void registerCallback( const KeyboardKeyCode &code, const KeyboardEventCallback &callback );

    void registerCallback( const MouseKeyCode &code, const MouseEventCallback &callback );

    void registerCallback( const int &gamepadIdx, const GamepadKeyCode &code, const GamepadKeyEventCallback &callback );

    void registerCallback( const int &gamepadIdx, const GamepadAxisEventCallback &callback );

    void registerMouseMoveCallback( const MouseMoveEventCallback &callback );

    void registerScrollCallback( const ScrollEventCallback &callback );

    inline KeyState checkKey( const KeyboardKeyCode &code )
    {
        return ( KeyState ) glfwGetKey( window, ( int ) code );
    }

    inline KeyState checkKey( const MouseKeyCode &code )
    {
        return ( KeyState ) glfwGetMouseButton( window, ( int ) code );
    }

    inline KeyState checkKey( const int &gamepadIdx, const GamepadKeyCode &code )
    {
        if ( gamepadIdx < currentGamepadStates.size( ) )
        {
            return KeyState::None;
        }

        return ( KeyState ) currentGamepadStates[ gamepadIdx ].buttons[ ( int ) code ];
    }

    void pollEvents( );

private:
    template< class tCode, class tMap, class tCallback >
    void registerEvent( const tCode &code, tMap &map, const tCallback &callback )
    {
        if ( map.find( code ) == map.end( ) )
        {
            map[ code ] = { };
        }

        map[ code ].emplace_back( callback );
    }
};

END_NAMESPACES

