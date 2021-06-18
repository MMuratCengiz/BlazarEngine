#pragma once

#include <BlazarCore/Common.h>
#include <BlazarCore/Time.h>
#include <BlazarECS/ECS.h>
#include <BlazarScene/World.h>
#include <BlazarScene/Scene.h>

#include "SampleSetupInputBindings.h"
#include "SampleFloor.h"
#include "SampleCar1.h"
#include "SampleCar2.h"
#include "SampleCubeMap.h"
#include "SampleCrate.h"
#include "SampleSmallCrate.h"
#include "SampleAnimatedFox.h"
#include "FpsCamera.h"
#include "SampleMovingCrate.h"
#include "boost/random.hpp"
#include "boost/generator_iterator.hpp"
#include <iostream>
#include <dirent.h>

using namespace BlazarEngine;

namespace Sample
{

class SampleGame_Small : public Scene::IPlayable
{
    Scene::World *world;

    std::shared_ptr< SampleCar1 > car1;
    std::shared_ptr< SampleCar2 > car2;
    std::shared_ptr< SampleAnimatedFox > animDummy;
    std::shared_ptr< ECS::IGameEntity > rocks;

    std::shared_ptr< Scene::Scene > initialScene;
    std::shared_ptr< FpsCamera > camera;
    std::shared_ptr< ECS::DynamicGameEntity > sceneLights;
    std::shared_ptr< ECS::DynamicGameEntity > cameraComponent;
    std::shared_ptr< SampleFloor > floor;
    std::shared_ptr< SampleCubeMap > sky;
    Input::ActionCallback inputCallback;
public:
    inline explicit SampleGame_Small( Scene::World *world ) : world( world )
    { }

    void init( ) override;
    void update( ) override;
    void dispose( ) override;
};

}

