#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <utility>
#include <functional>
#include <iostream>
#include "../graphics/RenderDevice.h"
#include "../core/Common.h"
#include "../input/GlobalEventHandler.h"
#include "../renderobjects/Triangle2D.h"
#include <chrono>

using namespace SomeVulkan::Input;
using namespace SomeVulkan::Graphics;

static void windowResizeCb( void *userPointer, int width, int height ) {
    if ( width > 0 && height > 0 ) {
        auto *pVulkanApi = static_cast< RenderDevice * >( userPointer );
//        pVulkanApi->resetSwapChain( );
    }
}

static uint32_t nowInSeconds( ) {
    return std::chrono::duration_cast< std::chrono::seconds >(
            std::chrono::system_clock::now( ).time_since_epoch( ) ).count( );
}

template< class Playable >
class Game {
private:
    Playable *playable;
    GLFWwindow *window;
    std::shared_ptr< RenderDevice > vk;

public:
    Game( int width, int height, const std::string &title, Playable *playable ) {
        if ( glfwInit( ) == GL_FALSE ) {
            const char *errorBuffer = new char[1024];
            glfwGetError( &errorBuffer );

            std::cout << "Unable to initialize GLFW: " << errorBuffer << std::endl;
        }

        glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );

#ifdef MACOS
        glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
#endif

        window = glfwCreateWindow( width, height, title.c_str( ), nullptr, nullptr );

        if ( window == nullptr ) {
            const char *errorBuffer = new char[1024];
            glfwGetError( &errorBuffer );

            std::cout << "Cannot initialize window!: " << errorBuffer << std::endl;
        }

        glfwMakeContextCurrent( window );
        glfwSwapInterval( 0 );

        std::function< void( double, double ) > f = [ & ]( double mouseX, double mouseY ) {
            playable->processMouseMove( mouseX, mouseY );
        };

        glfwSetWindowUserPointer( window, &f );
        glfwSetCursorPosCallback( window, [ ]( GLFWwindow *w, double mouseX, double mouseY ) {
            /*auto *f = ( std::function< void( double, double ) > * ) glfwGetWindowUserPointer( w );
            ( *f )( mouseX, mouseY );*/
        } );
#ifdef DEBUG
#endif
        vk = RenderDeviceBuilder::create( ).selectWindow( window ).selectDevice(
                [ ]( std::vector< DeviceInfo > devices ) -> DeviceInfo {
                    return devices[ 0 ];
                } ).create( );

    }

    void play( ) const {
        /*
         * Swap buffers is an important thing to remember here, how the graphics API works is that you should be constantly drawing into a buffer,
         * since filling this buffer with the whole image might take some time a method where you use double buffers is implemented, with double buffers,
         * you display the front buffer and the back buffer is whats actually being drawn own, once we're done drawing into the back buffer, we swap them
         * front buffer gets removed, back buffer becomes the front buffer, and a new back buffer is initialized.
         */


        GlobalEventHandler::Instance( ).addWindowResizeCallback( window, ( void * ) vk.get( ), &windowResizeCb );

        std::vector< Shader > shaders {
                Shader {
                        .type = ShaderType::Vertex,
                        .filename = PATH( "/shaders/spirv/vertex/default.spv" )
                },
                Shader {
                        .type = ShaderType::Fragment,
                        .filename = PATH( "/shaders/spirv/fragment/default.spv" )
                },
        };

        auto renderSurface = vk->createRenderSurface( shaders );
        auto renderer = renderSurface->getSurfaceRenderer( );

        this->playable->init( vk );

        auto start = nowInSeconds( );
        uint32_t fpsCounter = 0;

        while ( !glfwWindowShouldClose( window ) ) {
            playable->processEvents( window );

            int width, height;
            glfwGetFramebufferSize( window, &width, &height );

            if ( width > 0 && height > 0 ) {
                renderer->render( );
            }

            glfwSwapBuffers( window );
            glfwPollEvents( );

            if ( nowInSeconds( ) - start > 1 ) {
                start = nowInSeconds( );
                std::stringstream s;
                s << "FPS: " << fpsCounter;
                TRACE( COMPONENT_GAMEH, VERBOSITY_CRITICAL, s.str( ).c_str( ) );
                fpsCounter = 0;
            }

            fpsCounter++;
        }

        vk->beforeDelete( );
    }

    ~Game( ) {
        glfwDestroyWindow( window );
        glfwTerminate( );
    }
};