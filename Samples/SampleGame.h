#pragma once

#include "../Scene/World.h"
#include "../Scene/Scene.h"
#include "../Core/Time.h"
#include "../Scene/Camera.h"
#include "SampleHouse.h"
#include "SampleFloor.h"
#include "SampleCar1.h"
#include "SampleCar2.h"
#include "SampleOldHouse.h"
#include "SampleTrafficCone.h"
#include "SampleCubeMap.h"
#include "Spaceship.h"
#include "../ECS/CAmbientLight.h"
#include "../ECS/CDirectionalLight.h"
#include "../ECS/CPointLight.h"
#include "../ECS/CSpotLight.h"

using namespace BlazarEngine;

namespace Sample {

class SampleGame : public Scene::IPlayable {
    Scene::World * world;

    std::shared_ptr< ECS::CAmbientLight > ambientLight{ };
    std::shared_ptr< ECS::CDirectionalLight > directionalLight{ };
    std::shared_ptr< ECS::CPointLight > pointLight{ };
    std::shared_ptr< ECS::CSpotLight > spotLight{ };

    std::shared_ptr< Scene::Scene > initialScene;
    std::shared_ptr< Scene::FpsCamera > camera;
    std::shared_ptr< SampleCar1 > car1;
    std::shared_ptr< SampleCar2 > car2;
    std::shared_ptr< SampleTrafficCone > cone;
    std::shared_ptr< SampleFloor > floor;
    std::shared_ptr< SampleOldHouse > oldHouse;
    std::shared_ptr< SampleCubeMap > sky;
    Input::ActionCallback inputCallback;
public:
    inline explicit SampleGame( Scene::World * world ) : world( world ) { }
    void init( ) override;
    void update( ) override;
    void dispose( ) override;
};;

}

