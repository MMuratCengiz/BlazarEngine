#include "SampleGame.h"

namespace Sample
{

void SampleGame::init( )
{
    boost::mt19937 rng;
    boost::uniform_real< > range( -28.0f, 28.0f );
    boost::variate_generator< boost::mt19937, boost::uniform_real< >> randomRange( rng, range );

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

    cameraComponent = std::make_shared< ECS::DynamicGameEntity >( );
    cameraComponent->createComponent< ECS::CCamera >( );
    cameraComponent->getComponent< ECS::CCamera >( )->position = glm::vec3( -0.6f, 1.0f, 5.4f );
    camera = std::make_shared< FpsCamera >( cameraComponent->getComponent< ECS::CCamera >( ) );

    floor = std::make_shared< SampleFloor >( );
    car1 = std::make_shared< SampleCar1 >( world );
    car2 = std::make_shared< SampleCar2 >( world );
    sky = std::make_shared< SampleCubeMap >( );
    crate = std::make_shared< SampleCrate >( );
    sampleBall = std::make_shared< SampleBall >( );
    smallCrate = std::make_shared< SampleSmallCrate >( );

    rocks = std::make_shared< ECS::DynamicGameEntity >( );

    DIR *dir;
    struct dirent *ent;
    if ( ( dir = opendir( std::string( PATH( "/assets/models/Rocks/" ) ).c_str( ) ) ) != nullptr )
    {
        while ( ( ent = readdir( dir ) ) != nullptr )
        {
            if ( ent->d_namlen > 2 )
            {
                auto child = world->getAssetManager( )->createEntity( PATH( "/assets/models/Rocks/" ) + std::string(  ent->d_name ) );

                auto &texInfo = child->getChildren( )[ 0 ]->getComponent< BlazarEngine::ECS::CMaterial >( )->textures.emplace_back( BlazarEngine::ECS::Material::TextureInfo { } );
                texInfo.path = "/assets/textures/Rocks/Colorsheet Rock Grey.png";

                BlazarEngine::Physics::PhysicsTransformSystem::setPositionRecursive( child.get( ), glm::vec3( randomRange( ), 0.10, randomRange( ) ) );

                rocks->addChild( std::move( child ) );
            }
        }

        closedir( dir );
    }

    tree1 = world->getAssetManager( )->createEntity( PATH( "/assets/models/Tree Type1 04.dae" ) );

    for ( auto &child: tree1->getChildren( ) )
    {
        auto &texInfo = child->getComponent< BlazarEngine::ECS::CMaterial >( )->textures.emplace_back( BlazarEngine::ECS::Material::TextureInfo { } );
        texInfo.path = "/assets/textures/Colorsheet Tree Normal.png";
    }

    tree2 = world->getAssetManager( )->createEntity( PATH( "/assets/models/Tree Type3 04.dae" ) );

    for ( auto &child: tree2->getChildren( ) )
    {
        auto &texInfo = child->getComponent< BlazarEngine::ECS::CMaterial >( )->textures.emplace_back( BlazarEngine::ECS::Material::TextureInfo { } );
        texInfo.path = "/assets/textures/Colorsheet Tree Normal.png";
    }

    BlazarEngine::Physics::PhysicsTransformSystem::setPositionRecursive( tree1.get( ), glm::vec3( 4.0f, 0.10f, 2.0f ) );
    BlazarEngine::Physics::PhysicsTransformSystem::setRotationRecursive( tree1.get( ), { BlazarEngine::ECS::RotationUnit::Degrees, glm::vec3( -0.0f, 0.0f, 0.0f ) } );
    BlazarEngine::Physics::PhysicsTransformSystem::setScaleRecursive( tree1.get( ), glm::vec3( 0.5f, 0.5f, 0.5f ) );

    BlazarEngine::Physics::PhysicsTransformSystem::setPositionRecursive( tree2.get( ), glm::vec3( -4.0f, 0.10f, 2.0f ) );
    BlazarEngine::Physics::PhysicsTransformSystem::setRotationRecursive( tree2.get( ), { BlazarEngine::ECS::RotationUnit::Degrees, glm::vec3( -0.0f, 0.0f, 0.0f ) } );
    BlazarEngine::Physics::PhysicsTransformSystem::setScaleRecursive( tree2.get( ), glm::vec3( 0.5f, 0.5f, 0.5f ) );

    for ( int treeType = 0; treeType < 2; ++treeType )
    {
        auto &ptr = treeType == 0 ? tree1 : tree2;

        for ( int i = 0; i < 35; ++i )
        {
            auto instanceTransform = std::make_shared< ECS::CTransform >( );

            instanceTransform->position = glm::vec3( randomRange( ), 0.15, randomRange( ) );
            instanceTransform->scale = glm::vec3( 0.5f, 0.5f, 0.5f );

            BlazarEngine::Physics::PhysicsTransformSystem::addInstanceRecursive( ptr.get( ), instanceTransform );
        }
    }

    initialScene = std::make_shared< Scene::Scene >( );
    initialScene->addEntity( sceneLights );
    initialScene->addEntity( cameraComponent );
    initialScene->addEntity( car1 );
    initialScene->addEntity( car2 );
    initialScene->addEntity( tree1 );
    initialScene->addEntity( tree2 );
    initialScene->addEntity( floor );
    initialScene->addEntity( crate );
    initialScene->addEntity( rocks );
    initialScene->addEntity( smallCrate );
    initialScene->addEntity( sky );
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
            translation.z += Core::Time::getDeltaTime( ) * 40.0f;
        }
        if ( actionName == "MoveSmallCrate_Backwards" )
        {
            translation.z -= Core::Time::getDeltaTime( ) * 40.0f;
        }
        if ( actionName == "MoveSmallCrate_Left" )
        {
            translation.x -= Core::Time::getDeltaTime( ) * 40.0f;
        }
        if ( actionName == "MoveSmallCrate_Right" )
        {
            translation.x += Core::Time::getDeltaTime( ) * 40.0f;
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