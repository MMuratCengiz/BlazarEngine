#pragma once

#include <BlazarCore/Common.h>
#include <BlazarGraphics/RenderDevice.h>
#include <BlazarInput/GlobalEventHandler.h>
#include <string>
#include <utility>
#include <functional>
#include <iostream>
#include "../../Samples/BlazarSamples/FpsCamera.h"
#include <chrono>

NAMESPACES( ENGINE_NAMESPACE, Scene )

class Window
{
private:
    GLFWwindow *window;

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
    }

    void play( ) const
    {
        while ( !glfwWindowShouldClose( window ) )
        {
            Core::Time::tick( );

            int width, height;
            glfwGetFramebufferSize( window, &width, &height );

            if ( width > 0 && height > 0 )
            {

            }

            glfwSwapBuffers( window );
            glfwPollEvents( );
        }
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