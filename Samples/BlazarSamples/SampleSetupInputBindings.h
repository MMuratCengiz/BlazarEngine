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