//
// Created by Murat on 11/23/2020.
//

#include "SampleGame.h"

namespace Sample {

void SampleGame::init( ) {
    car1 = std::make_shared< SampleCar1 >( );
    car2 = std::make_shared< SampleCar2 >( );
    cone = std::make_shared< SampleTrafficCone >( );
    oldHouse = std::make_shared< SampleOldHouse >( );

    floor = std::make_shared< SampleFloor >( );
    initialScene.addEntity( car1 );
    initialScene.addEntity( car2 );
    initialScene.addEntity( cone );
    initialScene.addEntity( floor );
    initialScene.addEntity( oldHouse );
    world->setScene( initialScene );

    Input::ActionBinding binding { };
    binding.keyCode = Input::KeyboardKeyCode::T;
    binding.pressForm = Input::KeyPressForm::Pressed;
    binding.controller = Input::Controller::Keyboard;

/*    inputCallback = [&]( const std::string& actionName ) {
        auto transform = house->getComponent< ECS::CTransform >( );
        transform->position += glm::vec3( 0.0f, 0.0f, -Core::Time::getDeltaTime() * 0.001f );
    };

    world->getActionMap()->registerAction( "MoveHouse", binding, inputCallback );*/
}

void SampleGame::update( ) {

}

void SampleGame::dispose( ) {

}

}