#include <iostream>
#include <BlazarScene/World.h>
#include <BlazarSamples/SampleGame.h>
#include <BlazarSamples/SampleGame_Small.h>
#include <sol/sol.hpp>

using namespace BlazarEngine;
using namespace ECS;

int main( ) {
    std::cout << "=== opening a state ===" << std::endl;

    sol::state lua;
    // open some common libraries
    lua.open_libraries(sol::lib::base, sol::lib::package);
    lua.script("print('bark bark bark!')");

    sol::load_result main = lua.load_file("LuaSample/main.lua");
    main();

    std::unique_ptr< Scene::World > world = std::make_unique< Scene::World >( );
    world->init( 1920, 1080, "Some Vulkan" );

    auto game = std::make_shared< Sample::SampleGame_Small >( world.get( ) );
    world->run( std::static_pointer_cast< Scene::IPlayable >( game ) );

    return 0;
}
