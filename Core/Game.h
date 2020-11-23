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

using namespace SomeVulkan::Core;
using namespace SomeVulkan::Input;
using namespace SomeVulkan::Graphics;
using namespace SomeVulkan::Scene;
using namespace SomeVulkan::ECS;

static void windowResizeCb( void *userPointer, int width, int height ) {
    if ( width > 0 && height > 0 ) {
        auto *renderDevice = static_cast< RenderDevice * >( userPointer );
        renderDevice->getContext()->triggerEvent( EventType::SwapChainInvalidated );
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

        auto camera = std::make_shared< FpsCamera >( glm::vec3( 1.4f, 2.0f, -1.0f ), glm::vec3( 0.0f ) );

        std::vector< ShaderInfo > shaders( 2 );
        shaders[ 0 ].type = vk::ShaderStageFlagBits::eVertex;
		shaders[ 0 ].path = PATH( "/Shaders/SPIRV/Vertex/default.spv" );
        shaders[ 1 ].type = vk::ShaderStageFlagBits::eFragment;
		shaders[ 1 ].path = PATH( "/Shaders/SPIRV/Fragment/default.spv" );

        auto renderSurface = vk->createRenderSurface( shaders, camera );
        auto renderer = renderSurface->getSurfaceRenderer( );

        auto sampleHouse = std::make_shared< IGameEntity >( );
        auto mesh = sampleHouse->createComponent< CMesh >( );
        mesh->path = PATH( "/assets/models/viking_room.obj" );

        auto texture = sampleHouse->createComponent< CMaterial >( );
        auto &texInfo = texture->textures.emplace_back( Material::TextureInfo { } );
        texInfo.path = "/assets/textures/viking_room.png";

        auto transform = sampleHouse->createComponent< CTransform >( );
        transform->rotation.euler.x = -90.0f;

        renderer->addRenderObject( sampleHouse );

        this->playable->init( vk );

        auto start = nowInSeconds( );
        uint32_t fpsCounter = 0;

        int rotationIndex = 0;

        while ( !glfwWindowShouldClose( window ) ) {
            Time::tick();

            playable->processEvents( window );

            int width, height;
            glfwGetFramebufferSize( window, &width, &height );

            if ( width > 0 && height > 0 ) {
                renderer->render( );
            }

            glfwSwapBuffers( window );
            glfwPollEvents( );

            if ( glfwGetKey( window, GLFW_KEY_C ) == GLFW_PRESS ) {
                rotationIndex++;
                if ( rotationIndex >= 3 ) {
                    rotationIndex = 0;
                }
            }

            if ( glfwGetKey( window, GLFW_KEY_R ) == GLFW_PRESS ) {
                transform->rotation.euler[ rotationIndex ] += Time::getDeltaTime();
            }

            camera->processKeyboardEvents( window );
            camera->processMouseEvents( window);

            if ( nowInSeconds( ) - start > 1 ) {
                start = nowInSeconds( );
                std::stringstream s;
                s << "FPS: " << fpsCounter << " Camera Position " << camera->getPosition().x << camera->getPosition().y << camera->getPosition().z;
                TRACE( COMPONENT_GAMEH, VERBOSITY_CRITICAL, s.str( ).c_str( ) );

                fpsCounter = 0;
            }

            fpsCounter++;
        }

        vk->beforeDelete( );
        sampleHouse.reset();
    }

    ~Game( ) {
        glfwDestroyWindow( window );
        glfwTerminate( );
    }
};