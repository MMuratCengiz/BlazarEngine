#pragma once

#include "../Core/Common.h"
#include <string>
#include <utility>
#include <functional>
#include <iostream>
#include "../Graphics/RenderDevice.h"
#include "../Input/GlobalEventHandler.h"
#include "../Scene/FpsCamera.h"
#include <chrono>

NAMESPACES( SomeVulkan, Scene )

class Window {
private:
    GLFWwindow *window;

public:
    Window( const uint32_t& width, const uint32_t& height, const std::string &title ) {
        if ( glfwInit( ) == GL_FALSE ) {
            const char *errorBuffer = new char[1024];
            glfwGetError( &errorBuffer );
            TRACE( COMPONENT_GRAPHICS, VERBOSITY_CRITICAL, errorBuffer );
        }

        glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );

#ifdef MACOS
        glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
#endif

        window = glfwCreateWindow( width, height, title.c_str( ), nullptr, nullptr );

        if ( window == nullptr ) {
            const char *errorBuffer = new char[1024];
            glfwGetError( &errorBuffer );
            TRACE( COMPONENT_GRAPHICS, VERBOSITY_CRITICAL, errorBuffer );
        }

        glfwMakeContextCurrent( window );
        glfwSwapInterval( 0 );
    }

    void play( ) const {
        auto camera = std::make_shared< FpsCamera >( glm::vec3( 1.4f, 2.0f, -1.0f ), glm::vec3( 0.0f ) );

        while ( !glfwWindowShouldClose( window ) ) {
            Core::Time::tick();

            int width, height;
            glfwGetFramebufferSize( window, &width, &height );

            if ( width > 0 && height > 0 ) {

            }

            glfwSwapBuffers( window );
            glfwPollEvents( );

            camera->processKeyboardEvents( window );
            camera->processMouseEvents( window);
        }
    }

    GLFWwindow * getWindow() {
        return window;
    }

    ~Window( ) {
        glfwDestroyWindow( window );
        glfwTerminate( );
    }
};

END_NAMESPACES