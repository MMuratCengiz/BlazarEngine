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

#include <BlazarScene/World.h>

using namespace BlazarEngine;

namespace Sample
{

class SampleSetupInputBindings
{
public:
    static void setup( Scene::World *world )
    {
        world->getActionMap( )->registerAction( "MoveSmallCrate_Forward", { Input::KeyboardKeyCode::I } );
        world->getActionMap( )->registerAction( "MoveSmallCrate_Backwards", { Input::KeyboardKeyCode::K } );
        world->getActionMap( )->registerAction( "MoveSmallCrate_Left", { Input::KeyboardKeyCode::J } );
        world->getActionMap( )->registerAction( "MoveSmallCrate_Right", { Input::KeyboardKeyCode::L } );
    }
};

}