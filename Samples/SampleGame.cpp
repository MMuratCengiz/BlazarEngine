//
// Created by Murat on 11/23/2020.
//

#include "SampleGame.h"

namespace Sample {

void SampleGame::init( ) {
    ambientLight = std::make_shared< ECS::CAmbientLight >( );
    ambientLight->diffuse = glm::vec4( 0.0f, 0.25f, 0.1f, 1.0f);
    ambientLight->power = 0.01f;

    directionalLight = std::make_shared< ECS::CDirectionalLight >( );
    directionalLight->diffuse = glm::vec4( 0.2f, 0.4f, 0.3f, 1.0f );
    directionalLight->direction = glm::vec4( 2.0f, -2.0f, 1.0f, 1.0f );
    directionalLight->specular = glm::vec4( 0.0f );
    directionalLight->power = 0.8f;

    pointLight = std::make_shared< ECS::CPointLight >( );
    pointLight->position = glm::vec4( -0.6f, 1.5f, 7.4f, 1.0f );
    pointLight->specular = glm::vec4( 0.0f );
    pointLight->diffuse = glm::vec4( 0.5, 0.0f, 0.0f, 1.0f );
    pointLight->fillFromPower( 32 );

    spotLight = std::make_shared< ECS::CSpotLight >( );
    spotLight->position = glm::vec4( -0.6f, 0.5f, 5.4f, 1.0f );
    spotLight->diffuse = glm::vec4( 0.0f, 0.5f, 0.0f, 1.0f );
    spotLight->specular = glm::vec4( 0.0f );
    spotLight->power = 7;
    spotLight->radius = glm::cos( glm::radians( 12.5f ) );

//    oldHouse = std::make_shared< SampleOldHouse >( );
    camera = std::make_shared< Scene::FpsCamera >( glm::vec3( -0.6f, 1.0f, 5.4f ) );
    floor = std::make_shared< SampleFloor >( );
    car1 = std::make_shared< SampleCar1 >( );
    car2 = std::make_shared< SampleCar2 >( );
    cone = std::make_shared< SampleTrafficCone >( );
    sky = std::make_shared< SampleCubeMap >( );

    initialScene = std::make_shared< Scene::Scene >( camera );
    initialScene->addAmbientLight( ambientLight );
    initialScene->addDirectionalLight( directionalLight );
    initialScene->addPointLight( pointLight );
    initialScene->addSpotLight( spotLight );
    initialScene->addEntity( car1 );
    initialScene->addEntity( car2 );
    initialScene->addEntity( cone );
    initialScene->addEntity( floor );
//    initialScene->addEntity( oldHouse );
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

    Input::GlobalEventHandler::Instance().subscribeToEvent( Input::EventType::Tick, [&]( const Input::EventType& type, const Input::pEventParameters& parameters ) {
        auto tickParams = Input::GlobalEventHandler::ToTickParameters( parameters );

        camera->processKeyboardEvents( tickParams->window );
        camera->processMouseEvents( tickParams->window );

        spotLight->position = camera->getPosition();
        spotLight->direction = camera->getFront();
    });

}

void SampleGame::update( ) {

}

void SampleGame::dispose( ) {

}

}