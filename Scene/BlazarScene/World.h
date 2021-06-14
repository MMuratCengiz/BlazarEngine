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

    std::unique_ptr< Window > window;
    std::unique_ptr< Input::ActionMap > actionMap;
    std::unique_ptr< Physics::PhysicsWorld > physicsWorld { };
    std::unique_ptr< Physics::PhysicsTransformSystem > transformSystem { };
    std::unique_ptr< Graphics::AssetManager > assetManager;
    std::unique_ptr< Graphics::AnimationStateSystem > animationStateSystem;
    std::unique_ptr< Input::EventHandler > eventHandler;

    std::shared_ptr< Scene > currentScene;

    std::vector< std::unique_ptr< ECS::ISystem > > systems;
public:
    World( ) = default;

    void init( const uint32_t &windowWidth, const uint32_t &windowHeight, const std::string &title );

    void registerSystem( std::unique_ptr< ECS::ISystem > system );

    void setScene( std::shared_ptr< Scene > scene );

    void run( const std::shared_ptr< IPlayable > &game );

    inline const std::unique_ptr< Graphics::AssetManager > &getAssetManager( )
    {
        return assetManager;
    }

    inline const std::unique_ptr< Input::ActionMap > &getActionMap( )
    {
        return actionMap;
    }

    inline const std::unique_ptr< Physics::PhysicsTransformSystem > &getTransformSystem( )
    {
        return transformSystem;
    }

    // todo remove later
    inline GLFWwindow *getGLFWwindow( )
    {
        return window->getWindow( );
    }

    ~World( );
};

END_NAMESPACES