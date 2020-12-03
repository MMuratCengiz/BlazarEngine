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
//    std::shared_ptr< Graphics::Renderer > renderer;
    std::shared_ptr< Input::ActionMap > actionMap;
    std::shared_ptr< Input::EventHandler > eventHandler;
    std::shared_ptr< Scene > currentScene;
public:
    World() = default;

    void init( const uint32_t& windowWidth, const uint32_t& windowHeight, const std::string& title ) {
        window = std::make_unique< Window >( windowWidth, windowHeight, title );

        vk = Graphics::RenderDeviceBuilder::create( ).selectWindow( window->getWindow() ).selectDevice(
                [ ]( std::vector< Graphics::DeviceInfo > devices ) -> Graphics::DeviceInfo {
                    return devices[ 0 ];
                } ).create( );

        Input::GlobalEventHandler::Instance().initWindowEvents( window->getWindow() );

//        renderer = vk->getRenderer( );

        eventHandler = std::make_shared< Input::EventHandler >( window->getWindow() );
        actionMap = std::make_shared< Input::ActionMap >( eventHandler );
    }

    void setScene( const std::shared_ptr< Scene > &scene ) {
        vk->getRenderer()->setScene( scene );
        currentScene = scene;
    }

    void run( const std::shared_ptr< IPlayable >& game ) {
        game->init();

        GLFWwindow * glfwWindow = window->getWindow();
        FPSCounter fpsCounter = FPSCounter::Instance( );

        while ( !glfwWindowShouldClose( glfwWindow ) ) {
            Core::Time::tick();
            fpsCounter.tick();

            if ( glfwGetKey( glfwWindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) {
                glfwSetWindowShouldClose( glfwWindow, GL_TRUE );
            }

            int width, height;
            glfwGetFramebufferSize( glfwWindow, &width, &height );

            if ( width > 0 && height > 0 ) {
                vk->getRenderer()->render( );
            }

            glfwSwapBuffers( glfwWindow );
            glfwPollEvents( );

            eventHandler->pollEvents( );

            currentScene->getCamera()->processKeyboardEvents( glfwWindow );
            currentScene->getCamera()->processMouseEvents( glfwWindow );
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