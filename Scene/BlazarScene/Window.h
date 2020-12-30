#pragma once

#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <BlazarCore/Common.h>
#include <BlazarInput/GlobalEventHandler.h>
#include <string>
#include <utility>
#include <functional>
#include <iostream>
#include <chrono>

NAMESPACES( ENGINE_NAMESPACE, Scene )

class Window
{
private:
    GLFWwindow *window;
    std::unique_ptr< Graphics::RenderWindow > renderWindow;
public:
    Window( const uint32_t &width, const uint32_t &height, const std::string &title )
    {
        if ( glfwInit( ) == GL_FALSE )
        {
            const char *errorBuffer = new char[1024];
            glfwGetError( &errorBuffer );
            TRACE( "Graphics", VERBOSITY_CRITICAL, errorBuffer );
        }

        glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );

#ifdef MACOS
        glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
#endif

        window = glfwCreateWindow( width, height, title.c_str( ), nullptr, nullptr );

        if ( window == nullptr )
        {
            const char *errorBuffer = new char[1024];
            glfwGetError( &errorBuffer );
            TRACE( COMPONENT_GRAPHICS, VERBOSITY_CRITICAL, errorBuffer );
        }

        glfwMakeContextCurrent( window );
        glfwSwapInterval( 0 );

        uint32_t count;
        auto extensions = glfwGetRequiredInstanceExtensions( &count );

        std::vector< std::string > extensionsVec( count );

        for ( uint32_t index = 0; index < count; ++index )
        {
            extensionsVec[ index ] = std::string( extensions[ index ] );
        }

        int framebufferWidth, framebufferHeight;

        glfwGetFramebufferSize( window, &framebufferWidth, &framebufferHeight );

        renderWindow = std::make_unique< Graphics::RenderWindow >( framebufferWidth, framebufferHeight, extensionsVec );

#ifdef WIN32
        renderWindow->setPlatformSpecific( GetModuleHandle( nullptr ), glfwGetWin32Window( window ) );
#endif
    }

    Graphics::RenderWindow *getRenderWindow( )
    {
        return renderWindow.get( );
    }

    GLFWwindow *getWindow( )
    {
        return window;
    }

    ~Window( )
    {
        glfwDestroyWindow( window );
        glfwTerminate( );
    }
};

END_NAMESPACES