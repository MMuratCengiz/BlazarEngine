#pragma once

#include <BlazarECS/ECS.h>
#include <BlazarCore/Common.h>
#include <BlazarGraphics/RenderDevice.h>
#include <BlazarInput/GlobalEventHandler.h>
#include <BlazarPhysics/PhysicsWorld.h>
#include <BlazarPhysics/PhysicsTransformSystem.h>
#include <BlazarInput/ActionMap.h>

#include "Window.h"
#include "Scene.h"
#include "IPlayable.h"
#include "FPSCounter.h"
#include <chrono>
#include <string>
#include <utility>
#include <functional>
#include <iostream>

NAMESPACES( ENGINE_NAMESPACE, Scene )

class World
{
private:
    std::shared_ptr< Graphics::RenderDevice > vk { };

    std::unique_ptr< Window > window;
    std::shared_ptr< Input::ActionMap > actionMap;
    std::shared_ptr< Input::EventHandler > eventHandler;
    std::shared_ptr< Scene > currentScene;
    std::shared_ptr< Physics::PhysicsWorld > physicsWorld { };
    std::shared_ptr< Physics::PhysicsTransformSystem > transformSystem { };
public:
    World( ) = default;

    void init( const uint32_t &windowWidth, const uint32_t &windowHeight, const std::string &title )
    {
        window = std::make_unique< Window >( windowWidth, windowHeight, title );
        physicsWorld = std::make_shared< Physics::PhysicsWorld >( Physics::PhysicsWorldConfiguration { } );
        transformSystem = std::make_shared< Physics::PhysicsTransformSystem >( physicsWorld );

        vk = Graphics::RenderDeviceBuilder::create( ).selectWindow( window->getWindow( ) ).selectDevice(
                [ ]( std::vector< Graphics::DeviceInfo > devices ) -> Graphics::DeviceInfo
                {
                    return devices[ 0 ];
                } ).create( );

        Input::GlobalEventHandler::Instance( ).initWindowEvents( window->getWindow( ) );

        eventHandler = std::make_shared< Input::EventHandler >( window->getWindow( ) );
        actionMap = std::make_shared< Input::ActionMap >( eventHandler );
    }

    void setScene( const std::shared_ptr< Scene > &scene )
    {
        currentScene = scene;

        for ( const auto &entity: scene->getEntities( ) )
        {
            physicsWorld->addOrUpdateEntity( entity );
        }
    }

    void run( const std::shared_ptr< IPlayable > &game )
    {
        game->init( );

        GLFWwindow *glfwWindow = window->getWindow( );
        FPSCounter fpsCounter = FPSCounter::Instance( );

        while ( !glfwWindowShouldClose( glfwWindow ) )
        {
            Core::Time::tick( );
            fpsCounter.tick( );
            physicsWorld->tick( );

            vk->getRenderer( )->frameStart( currentScene->getComponentTable( ) );

            if ( glfwGetKey( glfwWindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
            {
                glfwSetWindowShouldClose( glfwWindow, GL_TRUE );
            }

            int width, height;
            glfwGetFramebufferSize( glfwWindow, &width, &height );

            if ( width > 0 && height > 0 )
            {
                for ( auto & entity: currentScene->getEntities( ) )
                {
                    vk->getRenderer( )->entityTick( entity );
                }
            }

            glfwSwapBuffers( glfwWindow );
            glfwPollEvents( );

            eventHandler->pollEvents( );

            auto tickParams = Input::GlobalEventHandler::createTickParameters( glfwWindow );
            Input::GlobalEventHandler::Instance( ).triggerEvent( Input::EventType::Tick, tickParams );

            vk->getRenderer( )->frameEnd( currentScene->getComponentTable( ) );
        }

        vk->beforeDelete( );
    }

    std::shared_ptr< Input::ActionMap > getActionMap( )
    {
        return actionMap;
    }

    std::shared_ptr< Physics::PhysicsTransformSystem > getTransformSystem( )
    {
        return transformSystem;
    }

    // todo remove later
    GLFWwindow* getGLFWwindow( )
    {
        return window->getWindow( );
    }

    void end( )
    {
        vk->beforeDelete( );
    }
};

END_NAMESPACES