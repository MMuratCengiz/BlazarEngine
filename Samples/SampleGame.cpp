//
// Created by Murat on 11/23/2020.
//

#include "SampleGame.h"

namespace Sample {

void SampleGame::init( ) {
    ambientLight.diffuse = glm::vec4( 0.5f, 0.0f, 0.0f, 1.0f);
    ambientLight.power = 1.0f;

    oldHouse = std::make_shared< SampleOldHouse >( );
    camera = std::make_shared< Scene::FpsCamera >( glm::vec3( -0.6f, 0.5f, 5.4f ) );
    floor = std::make_shared< SampleFloor >( );
    car1 = std::make_shared< SampleCar1 >( );
    car2 = std::make_shared< SampleCar2 >( );
    cone = std::make_shared< SampleTrafficCone >( );
    sky = std::make_shared< SampleCubeMap >( );

    initialScene = std::make_shared< Scene::Scene >( camera );
    initialScene->addAmbientLight( ambientLight );
    initialScene->addEntity( car1 );
    initialScene->addEntity( car2 );
    initialScene->addEntity( cone );
    initialScene->addEntity( floor );
    initialScene->addEntity( oldHouse );
    initialScene->addEntity( sky );
    world->setScene( initialScene );

    Input::ActionBinding binding { };
    binding.keyCode = Input::KeyboardKeyCode::T;
    binding.pressForm = Input::KeyPressForm::Pressed;
    binding.controller = Input::Controller::Keyboard;

    Input::GlobalEventHandler::Instance().subscribeToEvent( Input::EventType::WindowResized, [&]( const Input::EventType& type, const Input::pEventParameters& parameters ) {
        auto windowParams = Input::GlobalEventHandler::ToWindowResizedParameters( parameters );
        camera->updateAspectRatio( windowParams->width, windowParams->height );
    });
}

void SampleGame::update( ) {

}

void SampleGame::dispose( ) {

}

}