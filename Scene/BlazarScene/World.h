#pragma once

#include <BlazarECS/ECS.h>
#include <BlazarCore/Common.h>
#include <BlazarGraphics/VulkanBackend/VulkanDevice.h>
#include <BlazarGraphics/RenderGraph/GraphSystem.h>
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
    std::unique_ptr< Graphics::VulkanDevice > renderDevice { };

    std::unique_ptr< Window > window;
    std::unique_ptr< Input::ActionMap > actionMap;
    std::unique_ptr< Physics::PhysicsWorld > physicsWorld { };
    std::unique_ptr< Physics::PhysicsTransformSystem > transformSystem { };
    std::unique_ptr< Graphics::AssetManager > assetManager;
    std::unique_ptr< Input::EventHandler > eventHandler;

    std::shared_ptr< Scene > currentScene;

    std::vector< std::unique_ptr< ECS::ISystem > > systems;
public:
    World( ) = default;

    void init( const uint32_t &windowWidth, const uint32_t &windowHeight, const std::string &title )
    {
        window = std::make_unique< Window >( windowWidth, windowHeight, title );
        physicsWorld = std::make_unique< Physics::PhysicsWorld >( Physics::PhysicsWorldConfiguration { } );
        transformSystem = std::make_unique< Physics::PhysicsTransformSystem >( physicsWorld.get( ) );

        renderDevice = std::make_unique< Graphics::VulkanDevice >( );
        renderDevice->createDevice( window->getRenderWindow( ) );
        renderDevice->listDevices()[ 0 ].select( );

        Input::GlobalEventHandler::Instance( ).initWindowEvents( window->getWindow( ) );

        eventHandler = std::make_unique< Input::EventHandler >( window->getWindow( ) );
        actionMap = std::make_unique< Input::ActionMap >( eventHandler.get( ) );
        assetManager = std::make_unique< Graphics::AssetManager >( );

        auto graphSystem = std::make_unique< Graphics::GraphSystem >( renderDevice.get( ), assetManager.get( ) );
        registerSystem( std::move( graphSystem ) );
    }
    
    void registerSystem( std::unique_ptr< ECS::ISystem > system )
    {
        systems.push_back( std::move( system ) );
    }
    
    void setScene( std::shared_ptr< Scene > scene )
    {
        if ( currentScene != nullptr )
        {
            for ( const auto &entity: currentScene->getEntities( ) )
            {
                for ( auto &system: systems )
                {
                    system->removeEntity( entity );
                }
            }
        }
        
        currentScene = std::move( scene );

        for ( const auto &entity: currentScene->getEntities( ) )
        {
            for ( auto &system: systems )
            {
                system->addEntity( entity );    
            }
            
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

            for ( auto &system: systems )
            {
                system->frameStart( currentScene->getComponentTable( ) );
            }

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
                    for ( auto &system: systems )
                    {
                        system->entityTick( entity );
                    }
                }
            }

            glfwSwapBuffers( glfwWindow );
            glfwPollEvents( );

            eventHandler->pollEvents( );

            auto tickParams = Input::GlobalEventHandler::createTickParameters( glfwWindow );
            Input::GlobalEventHandler::Instance( ).triggerEvent( Input::EventType::Tick, tickParams );

            for ( auto &system: systems )
            {
                system->frameEnd( currentScene->getComponentTable( ) );
            }
        }

        renderDevice->beforeDelete( );
    }
    
    const std::unique_ptr< Graphics::AssetManager >& getAssetManager( )
    {
        return assetManager;
    }

    const std::unique_ptr< Input::ActionMap >& getActionMap( )
    {
        return actionMap;
    }

    const std::unique_ptr< Physics::PhysicsTransformSystem >& getTransformSystem( )
    {
        return transformSystem;
    }

    // todo remove later
    GLFWwindow* getGLFWwindow( )
    {
        return window->getWindow( );
    }

    ~World( )
    {
        renderDevice->beforeDelete( );

        transformSystem.reset( );
        physicsWorld.reset( );

        for ( auto& system: systems )
        {
            system->cleanup( );
        }

        systems.clear( );
        renderDevice.reset( );
    }
};

END_NAMESPACES