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
    static void setup( Scene::World * world )
    {
        Input::ActionBinding moveBoxForward { };
        moveBoxForward.keyCode = Input::KeyboardKeyCode::I;
        moveBoxForward.pressForm = Input::KeyPressForm::Pressed;
        moveBoxForward.controller = Input::Controller::Keyboard;

        Input::ActionBinding moveBoxBackwards { };
        moveBoxBackwards.keyCode = Input::KeyboardKeyCode::K;
        moveBoxBackwards.pressForm = Input::KeyPressForm::Pressed;
        moveBoxBackwards.controller = Input::Controller::Keyboard;

        Input::ActionBinding moveBoxLeft { };
        moveBoxLeft.keyCode = Input::KeyboardKeyCode::J;
        moveBoxLeft.pressForm = Input::KeyPressForm::Pressed;
        moveBoxLeft.controller = Input::Controller::Keyboard;

        Input::ActionBinding moveBoxRight { };
        moveBoxRight.keyCode = Input::KeyboardKeyCode::L;
        moveBoxRight.pressForm = Input::KeyPressForm::Pressed;
        moveBoxRight.controller = Input::Controller::Keyboard;

        world->getActionMap( )->registerAction( "MoveSmallCrate_Forward", moveBoxForward );
        world->getActionMap( )->registerAction( "MoveSmallCrate_Backwards", moveBoxBackwards );
        world->getActionMap( )->registerAction( "MoveSmallCrate_Left", moveBoxLeft );
        world->getActionMap( )->registerAction( "MoveSmallCrate_Right", moveBoxRight );
    }
};

}