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

#pragma once

#include <BlazarECS/ECS.h>
#include <BlazarCore/Common.h>
#include <BlazarGraphics/VulkanBackend/VulkanDevice.h>
#include <BlazarGraphics/RenderGraph/GraphSystem.h>
#include <BlazarGraphics/AnimationStateSystem.h>
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
    std::unique_ptr< ECS::IGameEntity > gameState { };

    std::unique_ptr< Window > window;
    std::unique_ptr< Input::ActionMap > actionMap;
    std::unique_ptr< Physics::PhysicsWorld > physicsWorld { };
    std::unique_ptr< Physics::PhysicsTransformSystem > transformSystem { };
    std::unique_ptr< Graphics::AssetManager > assetManager;
    std::unique_ptr< Graphics::AnimationStateSystem > animationStateSystem;
    std::unique_ptr< Graphics::GraphSystem > graphSystem;
    std::unique_ptr< Input::EventHandler > eventHandler;
    Scene * currentScene;

    std::vector< ECS::ISystem * > systems;
public:
    World( ) = default;

    void init( const uint32_t &windowWidth, const uint32_t &windowHeight, const std::string &title );

    void resize( const uint32_t &windowWidth, const uint32_t &windowHeight );
    void rename( const std::string &title );

    void registerSystem( ECS::ISystem * system );

    void setScene( Scene * scene );

    void run( IPlayable * game );

    inline Graphics::AssetManager* getAssetManager( )
    {
        return assetManager.get();
    }

    inline Input::ActionMap* getActionMap( )
    {
        return actionMap.get();
    }

    inline Graphics::GraphSystem* getGraphSystem()
    {
        return graphSystem.get( );
    }

    inline Physics::PhysicsTransformSystem* getTransformSystem( )
    {
        return transformSystem.get();
    }

    // todo remove later
    inline GLFWwindow *getGLFWwindow( )
    {
        return window->getWindow( );
    }

    ~World( );
};

END_NAMESPACES