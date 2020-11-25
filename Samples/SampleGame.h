#pragma once

#include "../Scene/World.h"
#include "../Scene/Scene.h"
#include "../Core/Time.h"
#include "SampleHouse.h"
#include "SampleFloor.h"

using namespace SomeVulkan;

namespace Sample {

class SampleGame : public Scene::IPlayable {
    Scene::World * world;
    Scene::Scene initialScene{ nullptr };
    std::shared_ptr< SampleHouse > house;
    std::shared_ptr< SampleFloor > floor;
    Input::ActionCallback inputCallback;
public:
    inline explicit SampleGame( Scene::World * world ) : world( world ) { }
    void init( ) override;
    void update( ) override;
    void dispose( ) override;
};;

}

