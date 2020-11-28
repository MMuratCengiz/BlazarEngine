#pragma once

#include "../Scene/World.h"
#include "../Scene/Scene.h"
#include "../Core/Time.h"
#include "SampleHouse.h"
#include "SampleFloor.h"
#include "SampleCar1.h"
#include "SampleCar2.h"
#include "SampleOldHouse.h"
#include "SampleTrafficCone.h"

using namespace SomeVulkan;

namespace Sample {

class SampleGame : public Scene::IPlayable {
    Scene::World * world;
    Scene::Scene initialScene{ nullptr };
    std::shared_ptr< SampleCar1 > car1;
    std::shared_ptr< SampleCar2 > car2;
    std::shared_ptr< SampleTrafficCone > cone;
    std::shared_ptr< SampleFloor > floor;
    std::shared_ptr< SampleOldHouse > oldHouse;
    Input::ActionCallback inputCallback;
public:
    inline explicit SampleGame( Scene::World * world ) : world( world ) { }
    void init( ) override;
    void update( ) override;
    void dispose( ) override;
};;

}

