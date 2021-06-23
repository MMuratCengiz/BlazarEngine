/*
Blazar Engine - 3D Game Engine
Copyright (c) 2020-2021 Muhammed Murat Cengiz

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
#include "World.h"

NAMESPACES( ENGINE_NAMESPACE, Scene )

void World::init( const uint32_t &windowWidth, const uint32_t &windowHeight, const std::string &title )
{
    window = std::make_unique< Window >( windowWidth, windowHeight, title );
    physicsWorld = std::make_unique< Physics::PhysicsWorld >( Physics::PhysicsWorldConfiguration { } );
    transformSystem = std::make_unique< Physics::PhysicsTransformSystem >( physicsWorld.get( ) );

    renderDevice = std::make_unique< Graphics::VulkanDevice >( );
    renderDevice->createDevice( window->getRenderWindow( ) );
    renderDevice->listDevices( )[ 0 ].select( );

    Input::GlobalEventHandler::Instance( ).initWindowEvents( window->getWindow( ) );

    eventHandler = std::make_unique< Input::EventHandler >( window->getWindow( ) );
    actionMap = std::make_unique< Input::ActionMap >( eventHandler.get( ) );
    assetManager = std::make_unique< Graphics::AssetManager >( );
    animationStateSystem = std::make_unique< Graphics::AnimationStateSystem >( assetManager.get( ) );

    auto graphSystem = std::make_unique< Graphics::GraphSystem >( renderDevice.get( ), assetManager.get( ) );
    registerSystem( std::move( graphSystem ) );
    registerSystem( std::move( animationStateSystem ) );
}

void World::registerSystem( std::unique_ptr< ECS::ISystem > system )
{
    systems.push_back( std::move( system ) );
}

void World::setScene( std::shared_ptr< Scene > scene )
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

    // * attach game state entity

    auto gameState = std::make_shared< ECS::DynamicGameEntity >( );
    gameState->createComponent< ECS::CGameState >( );

    auto gameStateComponent = gameState->getComponent< ECS::CGameState >( );
    gameStateComponent->surfaceWidth = window->getRenderWindow( )->getWidth( );
    gameStateComponent->surfaceHeight = window->getRenderWindow( )->getHeight( );

    Input::GlobalEventHandler::Instance( ).subscribeToEvent(
            Input::EventType::WindowResized,
            [ = ]( const Input::EventType &eventType, std::shared_ptr< Input::IEventParameters > parameters )
            {
                auto windowResizeParameters = Input::GlobalEventHandler::ToWindowResizedParameters( parameters );

                gameStateComponent->surfaceWidth = windowResizeParameters->width;
                gameStateComponent->surfaceHeight = windowResizeParameters->height;
            } );

    currentScene->addEntity( std::move( gameState ) );

    for ( const auto &entity: currentScene->getEntities( ) )
    {
        for ( auto &system: systems )
        {
            system->addEntity( entity );
        }

        physicsWorld->addOrUpdateEntity( entity );
    }
}

void World::run( const std::shared_ptr< IPlayable > &game )
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
            for ( auto &entity: currentScene->getEntities( ) )
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

World::~World( )
{
    Input::GlobalEventHandler::Instance( ).cleanup( );

    renderDevice->beforeDelete( );

    transformSystem.reset( );
    physicsWorld.reset( );

    for ( auto &system: systems )
    {
        system->cleanup( );
    }

    systems.clear( );
    renderDevice.reset( );
}

END_NAMESPACES

