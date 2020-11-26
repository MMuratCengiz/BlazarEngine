#pragma once

#include "../Core/Common.h"
#include "../Graphics/RenderDevice.h"
#include "../Input/GlobalEventHandler.h"
#include "../Scene/FpsCamera.h"
#include "Window.h"
#include "Scene.h"
#include "IPlayable.h"
#include "FPSCounter.h"
#include "../Input/ActionMap.h"
#include <chrono>
#include <string>
#include <utility>
#include <functional>
#include <iostream>

NAMESPACES( SomeVulkan, Scene )

class World {
private:
    std::shared_ptr< Graphics::RenderDevice > vk;

    std::unique_ptr< Window > window;
    std::shared_ptr< Graphics::RenderSurface > renderSurface;
    std::shared_ptr< Graphics::Renderer > renderer;
    std::shared_ptr< FpsCamera > camera;
    std::shared_ptr< Input::ActionMap > actionMap;
    std::shared_ptr< Input::EventHandler > eventHandler;
public:
    World() = default;

    void init( const uint32_t& windowWidth, const uint32_t& windowHeight, const std::string& title ) {
        window = std::make_unique< Window >( windowWidth, windowHeight, title );

        vk = Graphics::RenderDeviceBuilder::create( ).selectWindow( window->getWindow() ).selectDevice(
                [ ]( std::vector< Graphics::DeviceInfo > devices ) -> Graphics::DeviceInfo {
                    return devices[ 0 ];
                } ).create( );

        Input::GlobalEventHandler::Instance().initWindowEvents( window->getWindow() );
        camera = std::make_shared< FpsCamera >( glm::vec3( -0.6f, 3.2f, 5.4f ) );

        renderSurface = vk->createRenderSurface( camera );
        renderer = renderSurface->getSurfaceRenderer( );

        eventHandler = std::make_shared< Input::EventHandler >( window->getWindow() );
        actionMap = std::make_shared< Input::ActionMap >( eventHandler );
    }

    void setScene( const Scene &scene ) {
        for ( const auto& entity: scene.getEntities() ) {
            renderer->addRenderObject( entity );
        }
    }

    void run( const std::shared_ptr< IPlayable >& game ) {
        game->init();

        GLFWwindow * glfwWindow = window->getWindow();
        FPSCounter fpsCounter = FPSCounter::Instance( );

        Input::GlobalEventHandler::Instance().subscribeToEvent( Input::EventType::WindowResized, [&]( const Input::EventType& type, const Input::pEventParameters& parameters ) {
           auto windowParams = Input::GlobalEventHandler::ToWindowResizedParameters( parameters );
           camera->updateAspectRatio( windowParams->width, windowParams->height );
        });

        while ( !glfwWindowShouldClose( glfwWindow ) ) {
            Core::Time::tick();
            fpsCounter.tick();

            if ( glfwGetKey( glfwWindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) {
                glfwSetWindowShouldClose( glfwWindow, GL_TRUE );
            }

            int width, height;
            glfwGetFramebufferSize( glfwWindow, &width, &height );

            if ( width > 0 && height > 0 ) {
                renderer->render( );
            }

            glfwSwapBuffers( glfwWindow );
            glfwPollEvents( );

            eventHandler->pollEvents( );

            camera->processKeyboardEvents( glfwWindow );
            camera->processMouseEvents( glfwWindow );
        }

        vk->beforeDelete();
    }

    std::shared_ptr< Input::ActionMap >
            getActionMap() {
        return actionMap;
    }

    void end() {
        vk->beforeDelete( );
    }
};

END_NAMESPACES