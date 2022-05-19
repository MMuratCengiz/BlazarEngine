#include <iostream>
#include <BlazarScene/World.h>
#include <BlazarSamples/SampleGame_Small.h>
#ifdef DEBUG
#define SOL_ALL_SAFETIES_ON 1
#endif
#include <sol/sol.hpp>

using namespace BlazarEngine;
using namespace ECS;

class Dog {
public:
    std::string name;
    Dog(std::string name) : name(name) {}
};

class PetOwner {
public:
    Dog dog;
    std::string name;

    PetOwner(std::string name) : name(name), dog(Dog("")) {
    }
    void printData() {
        std::cout << "My name is " << name << ", and my pets name is: " << dog.name << std::endl;
    }
};

int main( ) {
    std::cout << "=== opening a state ===" << std::endl;

    sol::state lua;
    // open some common libraries
    lua.open_libraries(sol::lib::base, sol::lib::package);

    lua["print_bark"] = []() { std::cout << "bark!" << std::endl; };

    sol::usertype<PetOwner> petOwnerType = lua.new_usertype<PetOwner>("PetOwner", sol::constructors<PetOwner(std::string)>());
    sol::usertype<Dog> dogType = lua.new_usertype<Dog>("Dog",  sol::constructors<Dog(std::string)>());
    dogType["name"] = &Dog::name;
    petOwnerType["dog"] = &PetOwner::dog;
    petOwnerType["name"] = &PetOwner::name;
    petOwnerType["printData"] = &PetOwner::printData;

    lua.script_file("LuaSample/main.lua");

    std::unique_ptr< Scene::World > world = std::make_unique< Scene::World >( );
    world->init( 1920, 1080, "Some Vulkan" );

    auto game = std::make_unique< Sample::SampleGame_Small >( world.get( ) );
    world->run( game.get() );

    return 0;
}
