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
#include <BlazarECS/ECS.h>
#include <BlazarPhysics/PhysicsTransformSystem.h>
#include <BlazarScene/World.h>

namespace Sample
{

class SampleAnimatedFox : public BlazarEngine::ECS::IGameEntity
{
    BlazarEngine::Scene::World *world;
public:
    explicit SampleAnimatedFox( BlazarEngine::Scene::World * world );

    void iterChildren( const std::shared_ptr< BlazarEngine::ECS::IGameEntity >& entity );
};

}