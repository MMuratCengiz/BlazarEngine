#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include "2DGameScene.h"
#include "2DGamePasses.h"

using namespace BlazarEngine;
using namespace Graphics;

void TDGameScene::init( )
{
    camera = std::make_unique< TDGameTopDownCamera >( );
    box = std::move(
            world->getAssetManager( )->createEntity(
                    BuiltinPrimitives::getPrimitivePath( PrimitiveType::LitCube ) ) );

    auto *pTransform = box->getComponent< ECS::CTransform >( );
    pTransform->position.x = 960.0f;
    pTransform->position.y = 540.0f;
    pTransform->scale.x = 20.0f;
    pTransform->scale.y = 20.0f;

    boost::mt19937 rng;
    boost::uniform_real< > rangeX( 0.0f, 1920.0f );
    boost::uniform_real< > rangeY( 0.0f, 1080.0f );
    boost::variate_generator< boost::mt19937, boost::uniform_real< > > randomRangeX( rng, rangeX );
    boost::variate_generator< boost::mt19937, boost::uniform_real< > > randomRangeY( rng, rangeY );

    box->createComponent< ECS::CInstances >( );
    for (int i = 0; i < 100; ++i)
    {
        ECS::CTransform instance { };
        instance.position.x = randomRangeX( );
        instance.position.y = randomRangeY( );
        instance.scale.x = 20.0f;
        instance.scale.y = 20.0f;
        box->getComponent< ECS::CInstances >( )
                ->transforms.push_back( std::move( instance ) );
    }

    auto boxMaterial = box->getComponent< ECS::CMaterial >( );
    ECS::Material::TextureInfo & boxTexture = boxMaterial->textures.emplace_back( );
    box->getComponent< ECS::CMesh >( )->cullMode = ECS::CullMode::None;
    boxTexture.path = PATH( "/assets/textures/floor2.png" );

    scene = std::make_unique< Scene::Scene >( );

    scene->addEntity( camera.get( ));
    scene->addEntity( box.get( ));

    presentPass = TDGamePasses::createPresentPass( );
    world->getGraphSystem( )->addPass( presentPass.get( ));
    world->setScene( scene.get( ) );
    srand (time(NULL));

    auto movement = [ = ]( const std::string &actionName )
    {
        glm::vec3 translation { };
        double velocity = 200;
        if ( actionName == "Right" )
        {
            pTransform->position.x += Core::Time::getDeltaTime( ) * velocity;
        }
        if ( actionName == "Left" )
        {
            pTransform->position.x -= Core::Time::getDeltaTime( ) * velocity;
        }
        if ( actionName == "Forward" )
        {
            pTransform->position.y += Core::Time::getDeltaTime( ) * velocity;
        }
        if ( actionName == "Backward" )
        {
            pTransform->position.y -= Core::Time::getDeltaTime( ) * velocity;
        }
    };

    Input::ActionBinding moveBoxForward { };
    moveBoxForward.keyCode = Input::KeyboardKeyCode::W;
    moveBoxForward.pressForm = Input::KeyPressForm::Pressed;
    moveBoxForward.controller = Input::Controller::Keyboard;
    Input::ActionBinding moveBoxBackward { };
    moveBoxBackward.keyCode = Input::KeyboardKeyCode::S;
    moveBoxBackward.pressForm = Input::KeyPressForm::Pressed;
    moveBoxBackward.controller = Input::Controller::Keyboard;
    Input::ActionBinding moveBoxRight { };
    moveBoxRight.keyCode = Input::KeyboardKeyCode::D;
    moveBoxRight.pressForm = Input::KeyPressForm::Pressed;
    moveBoxRight.controller = Input::Controller::Keyboard;
    Input::ActionBinding moveBoxLeft { };
    moveBoxLeft.keyCode = Input::KeyboardKeyCode::A;
    moveBoxLeft.pressForm = Input::KeyPressForm::Pressed;
    moveBoxLeft.controller = Input::Controller::Keyboard;
    Input::ActionBinding leftClick { };
    moveBoxLeft.keyCode = Input::KeyboardKeyCode::A;
    moveBoxLeft.pressForm = Input::KeyPressForm::Pressed;
    moveBoxLeft.controller = Input::Controller::Keyboard;

    world->getActionMap( )->registerAction( "Right", moveBoxRight );
    world->getActionMap( )->registerAction( "Left", moveBoxLeft );
    world->getActionMap( )->registerAction( "Forward", moveBoxForward );
    world->getActionMap( )->registerAction( "Backward", moveBoxBackward );
    world->getActionMap( )->subscribeToAction( "Right", movement );
    world->getActionMap( )->subscribeToAction( "Left", movement );
    world->getActionMap( )->subscribeToAction( "Forward", movement );
    world->getActionMap( )->subscribeToAction( "Backward", movement );
}

void TDGameScene::update( )
{

}

void TDGameScene::dispose( )
{

}