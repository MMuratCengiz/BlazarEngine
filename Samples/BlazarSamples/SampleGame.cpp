#include "SampleGame.h"

namespace Sample
{

void SampleGame::init( )
{
    sceneLights = std::make_shared< ECS::DynamicGameEntity >( );

    sceneLights->createComponent< ECS::CAmbientLight >( );
    sceneLights->getComponent< ECS::CAmbientLight >( )->diffuse = glm::vec4( 0.25f, 0.25f, 0.22f, 1.0f );
    sceneLights->getComponent< ECS::CAmbientLight >( )->power = 0.01f;

    glm::vec3 pos = glm::vec3( 10.4072, 11.5711, -9.09731 );
    glm::vec3 front = glm::vec3( -0.68921, -0.48481, 0.53847 );

    sceneLights->createComponent< ECS::CDirectionalLight >( );
    sceneLights->getComponent< ECS::CDirectionalLight >( )->diffuse = glm::vec4( 0.65f, 1.0f, 0.65f, 1.0f );
    sceneLights->getComponent< ECS::CDirectionalLight >( )->direction = glm::vec4( front - pos, 1.0f );
    sceneLights->getComponent< ECS::CDirectionalLight >( )->specular = glm::vec4( 0.0f );
    sceneLights->getComponent< ECS::CDirectionalLight >( )->power = 0.8f;

/*
    sceneLights[ 2 ]->createComponent< ECS::CPointLight >( );
    sceneLights[ 2 ]->getComponent< ECS::CPointLight >( )->position = glm::vec4( -0.6f, 1.5f, 7.4f, 1.0f );
    sceneLights[ 2 ]->getComponent< ECS::CPointLight >( )->specular = glm::vec4( 0.0f );
    sceneLights[ 2 ]->getComponent< ECS::CPointLight >( )->diffuse = glm::vec4( 0.5, 0.0f, 0.0f, 1.0f );
    sceneLights[ 2 ]->getComponent< ECS::CPointLight >( )->fillFromPower( 32 );
    sceneLights[ 3 ]->createComponent< ECS::CSpotLight >( );
    sceneLights[ 3 ]->getComponent< ECS::CSpotLight >( )->position = glm::vec4( -0.6f, 0.5f, 5.4f, 1.0f );
    sceneLights[ 3 ]->getComponent< ECS::CSpotLight >( )->diffuse = glm::vec4( 0.0f, 0.5f, 0.0f, 1.0f );
    sceneLights[ 3 ]->getComponent< ECS::CSpotLight >( )->specular = glm::vec4( 0.0f );
    sceneLights[ 3 ]->getComponent< ECS::CSpotLight >( )->power = 7;
    sceneLights[ 3 ]->getComponent< ECS::CSpotLight >( )->radius = glm::cos( glm::radians( 12.5f ) );*/

    cameraComponent = std::make_shared< ECS::DynamicGameEntity >( );
    cameraComponent->createComponent< ECS::CCamera >( );
    cameraComponent->getComponent< ECS::CCamera >( )->position = glm::vec3( -0.6f, 1.0f, 5.4f );
    camera = std::make_shared< FpsCamera >( cameraComponent->getComponent< ECS::CCamera >( ) );

    sampleMovingCrate = std::make_shared< SampleMovingCrate >( );
    floor = std::make_shared< SampleFloor >( );
    car1 = std::make_shared< SampleCar1 >( world );
    car2 = std::make_shared< SampleCar2 >( world );
//    cone = std::make_shared< SampleTrafficCone >( world );
    sky = std::make_shared< SampleCubeMap >( );
    crate = std::make_shared< SampleCrate >( );
    sampleBall = std::make_shared< SampleBall >( );
//    sampleWolf = std::make_shared< SampleAnimatedWolf >( );
    smallCrate = std::make_shared< SampleSmallCrate >( );

    initialScene = std::make_shared< Scene::Scene >( );
    initialScene->addEntity( sceneLights );
    initialScene->addEntity( cameraComponent );
    initialScene->addEntity( car1 );
    initialScene->addEntity( car2 );
//    initialScene->addEntity( cone );
    initialScene->addEntity( floor );
    initialScene->addEntity( crate );
    initialScene->addEntity( smallCrate );
//    initialScene->addEntity( sky );
//    initialScene->addEntity( sampleWolf );
//    initialScene->addEntity( sampleMovingCrate );
    world->setScene( initialScene );

    Input::GlobalEventHandler::Instance( ).subscribeToEvent( Input::EventType::WindowResized, [ & ]( const Input::EventType &type, const Input::pEventParameters &parameters )
    {
        auto windowParams = Input::GlobalEventHandler::ToWindowResizedParameters( parameters );
        camera->updateAspectRatio( windowParams->width, windowParams->height );
    } );

    Input::GlobalEventHandler::Instance( ).subscribeToEvent( Input::EventType::Tick, [ & ]( const Input::EventType &type, const Input::pEventParameters &parameters )
    {
        auto tickParams = Input::GlobalEventHandler::ToTickParameters( parameters );

        camera->processKeyboardEvents( tickParams->window );
        camera->processMouseEvents( tickParams->window );
    } );

    SampleSetupInputBindings::setup( world );

    auto movement = [ = ]( const std::string &actionName )
    {
        glm::vec3 translation { };

        if ( actionName == "MoveSmallCrate_Forward" )
        {
            translation.z += Core::Time::getDeltaTime( ) * 4.0f;
        }
        if ( actionName == "MoveSmallCrate_Backwards" )
        {
            translation.z -= Core::Time::getDeltaTime( ) * 4.0f;
        }
        if ( actionName == "MoveSmallCrate_Left" )
        {
            translation.x -= Core::Time::getDeltaTime( ) * 4.0f;
        }
        if ( actionName == "MoveSmallCrate_Right" )
        {
            translation.x += Core::Time::getDeltaTime( ) * 4.0f;
        }

        world->getTransformSystem( )->translate( smallCrate, translation );
    };

    world->getActionMap( )->subscribeToAction( "MoveSmallCrate_Forward", movement );
    world->getActionMap( )->subscribeToAction( "MoveSmallCrate_Backwards", movement );
    world->getActionMap( )->subscribeToAction( "MoveSmallCrate_Left", movement );
    world->getActionMap( )->subscribeToAction( "MoveSmallCrate_Right", movement );
}

void SampleGame::update( )
{
    camera->processKeyboardEvents( world->getGLFWwindow( ) );
    camera->processMouseEvents( world->getGLFWwindow( ) );
}

void SampleGame::dispose( )
{

}

}