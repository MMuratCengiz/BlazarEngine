#pragma once

#include <BlazarCore/Common.h>
#include <BlazarCore/Time.h>
#include <BlazarECS/ECS.h>
#include <BlazarScene/World.h>
#include <BlazarScene/Scene.h>

#include "SampleSetupInputBindings.h"
#include "SampleHouse.h"
#include "SampleFloor.h"
#include "SampleCar1.h"
#include "SampleCar2.h"
#include "SampleOldHouse.h"
#include "SampleTrafficCone.h"
#include "SampleCubeMap.h"
#include "Spaceship.h"
#include "SampleCrate.h"
#include "SampleSmallCrate.h"
#include "SampleBall.h"
#include "FpsCamera.h"
#include "SampleAnimatedWolf.h"
#include "SampleMovingCrate.h"

using namespace BlazarEngine;

namespace Sample
{

class SampleGame : public Scene::IPlayable
{
    Scene::World *world;

    std::shared_ptr< Scene::Scene > initialScene;
    std::shared_ptr< FpsCamera > camera;
    std::vector< std::shared_ptr< ECS::DynamicGameEntity > > sceneLights;
    std::shared_ptr< ECS::DynamicGameEntity > cameraComponent;
    std::shared_ptr< SampleCar1 > car1;
    std::shared_ptr< SampleCar2 > car2;
    std::shared_ptr< SampleTrafficCone > cone;
    std::shared_ptr< SampleFloor > floor;
    std::shared_ptr< SampleOldHouse > oldHouse;
    std::shared_ptr< SampleCubeMap > sky;
    std::shared_ptr< SampleCrate > crate;
    std::shared_ptr< SampleSmallCrate > smallCrate;
    std::shared_ptr< SampleBall > sampleBall;
    std::shared_ptr< SampleAnimatedWolf > sampleWolf;
    std::shared_ptr< SampleMovingCrate > sampleMovingCrate;
    Input::ActionCallback inputCallback;
public:
    inline explicit SampleGame( Scene::World *world ) : world( world )
    { }

    void init( ) override;
    void update( ) override;
    void dispose( ) override;
};

}

