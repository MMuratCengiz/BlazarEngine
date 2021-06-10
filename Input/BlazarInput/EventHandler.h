#pragma once

#include <BlazarCore/Common.h>

#define GLFW_INCLUDE_VULKAN
#ifdef __APPLE_CC__
#elif
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

