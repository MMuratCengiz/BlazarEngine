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

#include <BlazarScene/Window.h>
#include <BlazarScene/Scene.h>
#include <BlazarScene/IPlayable.h>
#include <BlazarScene/FPSCounter.h>
#include <chrono>
#include <string>
#include <utility>
#include <functional>
#include <iostream>
#include <BlazarScene/World.h>

NAMESPACES( ENGINE_NAMESPACE, Scene )

void World::init( const uint32_t &windowWidth, const uint32_t &windowHeight, const std::string &title )
{
    window = std::make_unique< Window >( windowWidth, windowHeight, title );
    physicsWorld = std::make_unique< Physics::PhysicsWorld >( Physics::PhysicsWorldConfiguration { } );
    transformSystem = std::make_unique< Physics::PhysicsTransformSystem >( physicsWorld.get( ) );

    renderDevice = std::make_unique< Graphics::VulkanDevice >( );
    renderDevice->createDevice( window->getRenderWindow( ) );
    renderDevice->listDevices( )[ 0 ].select( );

    Input::Events::initWindowEvents( window->getWindow( ) );

    eventHandler = std::make_unique< Input::EventHandler >( window->getWindow( ) );
    actionMap = std::make_unique< Input::ActionMap >( eventHandler.get( ) );
    assetManager = std::make_unique< Graphics::AssetManager >( );
    animationStateSystem = std::make_unique< Graphics::AnimationStateSystem >( assetManager.get( ) );
    graphSystem = std::make_unique< Graphics::GraphSystem >( renderDevice.get( ), assetManager.get( ) );

    registerSystem( graphSystem.get( ) );
    registerSystem( animationStateSystem.get( ) );
}

void World::resize( const uint32_t &windowWidth, const uint32_t &windowHeight )
{

}

void World::rename( const std::string &title )
{

}

void World::registerSystem( ECS::ISystem *system )
{
    systems.push_back( system );
}

void World::setScene( Scene *scene )
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

    currentScene = scene;

    // * attach game state entity

    gameState = std::make_unique< ECS::DynamicGameEntity >( );
    gameState->createComponent< ECS::CGameState >( );

    auto gameStateComponent = gameState->getComponent< ECS::CGameState >( );
    gameStateComponent->surfaceWidth = window->getRenderWindow( )->getWidth( );
    gameStateComponent->surfaceHeight = window->getRenderWindow( )->getHeight( );

    Input::Events::subscribe< Input::WindowResizedParameters * >(
            Input::EventType::WindowResized,
            [ = ]( Input::WindowResizedParameters *windowResizeParameters )
            {
                gameStateComponent->surfaceWidth = windowResizeParameters->width;
                gameStateComponent->surfaceHeight = windowResizeParameters->height;
            } );

    currentScene->addEntity( gameState.get( ) );

    for ( const auto &entity: currentScene->getEntities( ) )
    {
        for ( auto &system: systems )
        {
            system->addEntity( entity );
        }

        physicsWorld->addOrUpdateEntity( entity );
    }
}

void World::run( IPlayable *game )
{
    game->init( );

    GLFWwindow *glfwWindow = window->getWindow( );
    FPSCounter fpsCounter = FPSCounter::Instance( );

    int width, height;
    Input::Events::subscribe< Input::WindowResizedParameters * >( Input::EventType::WindowResized, [ & ]( Input::WindowResizedParameters *windowResizeParameters )
    {
        width = windowResizeParameters->width;
        height = windowResizeParameters->height;
    } );

    Input::TickParameters * tickParameters = new Input::TickParameters { };
    tickParameters->window = glfwWindow;
    auto tickParams = std::unique_ptr< Input::TickParameters >( tickParameters );

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

        glfwPollEvents( );

        eventHandler->pollEvents( );

        Input::Events::trigger( Input::EventType::Tick, tickParams.get( ) );

        for ( auto &system: systems )
        {
            system->frameEnd( currentScene->getComponentTable( ) );
        }
    }

    renderDevice->beforeDelete( );
}

World::~World( )
{
    Input::GlobalEventHandler< Input::TickParameters * >::Instance( ).cleanup( );
    Input::GlobalEventHandler< Input::WindowResizedParameters * >::Instance( ).cleanup( );
    Input::GlobalEventHandler< Input::SwapChainInvalidatedParameters * >::Instance( ).cleanup( );

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

