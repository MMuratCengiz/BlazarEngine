/*
Blazar Engine - 3D Game Engine
Copyright (c) 2020-2021 Muhammed Murat Cengiz

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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

using namespace BlazarEngine;

namespace Sample
{

class SampleGame_Small : public Scene::IPlayable
{
    Scene::World *world;

    std::unique_ptr< SampleCar1 > car1;
    std::unique_ptr< SampleCar2 > car2;
    std::unique_ptr< SampleAnimatedFox > animDummy;
    std::unique_ptr< ECS::IGameEntity > rocks;

    std::unique_ptr< Scene::Scene > initialScene;
    std::unique_ptr< FpsCamera > camera;
    std::unique_ptr< ECS::DynamicGameEntity > sceneLights;
    std::unique_ptr< ECS::DynamicGameEntity > cameraComponent;
    std::unique_ptr< SampleFloor > floor;
    std::unique_ptr< SampleCubeMap > sky;
    Input::ActionCallback inputCallback;
public:
    inline explicit SampleGame_Small( Scene::World *world ) : world( world )
    { }

    void init( ) override;
    void update( ) override;
    void dispose( ) override;
};

}

