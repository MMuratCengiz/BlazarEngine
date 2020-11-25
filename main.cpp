#include <iostream>
#include "Scene/World.h"
#include "Samples/SampleGame.h"

using namespace SomeVulkan;
using namespace ECS;

int main( ) {
    std::unique_ptr< Scene::World > world = std::make_unique< Scene::World >( );
    world->init( 800, 600, "Some Vulkan" );

    auto game = std::make_shared< Sample::SampleGame >( world.get() );
    world->run( std::static_pointer_cast< Scene::IPlayable >( game ) );

    return 0;
}
