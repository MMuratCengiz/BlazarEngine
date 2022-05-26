#include <BlazarScene/World.h>
#include <BlazarSamples/SampleGame2D/2DGameScene.h>

using namespace BlazarEngine;
using namespace ECS;

#include <filesystem>
#include <iostream>

int main( )
{
    std::unique_ptr< Scene::World > world = std::make_unique< Scene::World >( );
    world->init( 1920, 1080, "Some Vulkan" );

    auto game = std::make_unique< TDGameScene >( world.get( ) );
    world->run( game.get( ) );

    return 0;
}
