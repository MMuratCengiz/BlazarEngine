#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <BlazarSamples/SampleGame2D/2DGameScene.h>
#include <BlazarSamples/SampleGame2D/2DGamePasses.h>

using namespace BlazarEngine;
using namespace Graphics;

void TDGameScene::init( )
{
    camera = std::make_unique< TDGameTopDownCamera >( world );
    box = std::move(
            world->getAssetManager( )->createEntity(
                    BuiltinPrimitives::getPrimitivePath( PrimitiveType::TexturedSquare ) ) );

    auto *pTransform = box->getComponent< ECS::CTransform >( );
    pTransform->position.x = 960.0f;
    pTransform->position.y = 540.0f;
    pTransform->scale.x = 200.0f;
    pTransform->scale.y = 200.0f;

    auto boxMaterial = box->getComponent< ECS::CMaterial >( );
    ECS::Material::TextureInfo & boxTexture = boxMaterial->textures.emplace_back( );
    box->getComponent< ECS::CMesh >( )->cullMode = ECS::CullMode::None;
    boxTexture.path = "assets/textures/wood_box_1.png";

    scene = std::make_unique< Scene::Scene >( );

    scene->addEntity( camera.get( ) );
    scene->addEntity( box.get( ));

    presentPass = TDGamePasses::createPresentPass( );
    world->getGraphSystem( )->addPass( presentPass.get( ));
    world->setScene( scene.get( ) );
    srand (time(NULL));

    auto movement = [ = ]( const std::string &actionName, const Input::KeyState &keyPressForm, const float &pressure )
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

    world->getActionMap( )->registerAction( "Right", { Input::KeyboardKeyCode::D } );
    world->getActionMap( )->registerAction( "Left", { Input::KeyboardKeyCode::A } );
    world->getActionMap( )->registerAction( "Forward", { Input::KeyboardKeyCode::W } );
    world->getActionMap( )->registerAction( "Backward", { Input::KeyboardKeyCode::S } );
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