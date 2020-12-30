#include <iostream>
#include <BlazarScene/World.h>
#include <BlazarSamples/SampleGame.h>

using namespace BlazarEngine;
using namespace ECS;

int main( ) {
    std::unique_ptr< Scene::World > world = std::make_unique< Scene::World >( );
    world->init( 2560, 1440, "Some Vulkan" );

    auto game = std::make_shared< Sample::SampleGame >( world.get() );
    world->run( std::static_pointer_cast< Scene::IPlayable >( game ) );

    return 0;
}
