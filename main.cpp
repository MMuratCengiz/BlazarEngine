#include <iostream>
#include <BlazarScene/World.h>
#include <BlazarSamples/SampleGame.h>
#include <BlazarSamples/SampleGame_Small.h>
#include <lua.hpp>

using namespace BlazarEngine;
using namespace ECS;

int main( ) {
    lua_State * state = luaL_newstate();
    std::unique_ptr< Scene::World > world = std::make_unique< Scene::World >( );
    world->init( 1920, 1080, "Some Vulkan" );

    auto game = std::make_shared< Sample::SampleGame_Small >( world.get( ) );
    world->run( std::static_pointer_cast< Scene::IPlayable >( game ) );

    return 0;
}
