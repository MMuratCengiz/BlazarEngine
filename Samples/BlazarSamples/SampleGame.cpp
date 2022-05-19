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

#include "SampleGame.h"

namespace Sample
{
void SampleGame::init( )
{
    boost::mt19937 rng;
    boost::uniform_real< > range( -28.0f, 28.0f );
    boost::variate_generator< boost::mt19937, boost::uniform_real< > > randomRange( rng, range );

    sceneLights = std::make_unique< ECS::DynamicGameEntity >( );
    sceneLights->createComponent< ECS::CAmbientLight >( );
    sceneLights->getComponent< ECS::CAmbientLight >( )->diffuse = glm::vec3( 0.25f, 0.25f, 0.22f );
    sceneLights->getComponent< ECS::CAmbientLight >( )->power = 0.005f;

    auto pos = glm::vec3( 1, 1, 1 );
    auto front = glm::vec3( 0.0, 0.0, 0.0 );

    sceneLights->createComponent< ECS::CDirectionalLight >( );
    sceneLights->getComponent< ECS::CDirectionalLight >( )->diffuse = glm::vec3( 1.0, 1.0f, 0.99f );
    sceneLights->getComponent< ECS::CDirectionalLight >( )->direction = front - pos;
    sceneLights->getComponent< ECS::CDirectionalLight >( )->specular = glm::vec3( 1.0, 1.0f, 0.99f );
    sceneLights->getComponent< ECS::CDirectionalLight >( )->power = 0.15f;

    cameraComponent = std::make_unique< ECS::DynamicGameEntity >( );
    cameraComponent->createComponent< ECS::CCamera >( );
    cameraComponent->getComponent< ECS::CCamera >( )->position = glm::vec3( -0.6f, 1.0f, 5.4f );
    camera = std::make_unique< FpsCamera >( cameraComponent->getComponent< ECS::CCamera >( ) );

    floor = std::make_unique< SampleFloor >( world );
    car1 = std::make_unique< SampleCar1 >( world );
    car2 = std::make_unique< SampleCar2 >( world );
    sky = std::make_unique< SampleCubeMap >( world );
    crate = std::make_unique< SampleCrate >( );
    smallCrate = std::make_unique< SampleSmallCrate >( );
    animDummy = std::make_unique< SampleAnimatedFox >( world );

    rocks = std::make_unique< ECS::DynamicGameEntity >( );

    /*
    DIR *dir;
    struct dirent *ent;
    if ( ( dir = opendir( std::string( PATH( "/assets/models/Rocks/" ) ).c_str( ) ) ) != nullptr )
    {
        while ( ( ent = readdir( dir ) ) != nullptr )
        {
            if ( ent->d_namlen > 2 )
            {
                auto child = world->getAssetManager( )->createEntity( PATH( "/assets/models/Rocks/" ) + std::string( ent->d_name ) );

                auto &texInfo = child->getChildren( )[ 0 ]->getComponent< BlazarEngine::ECS::CMaterial >( )->textures.emplace_back( BlazarEngine::ECS::Material::TextureInfo { } );
                texInfo.path = PATH( "/assets/textures/Rocks/Colorsheet Rock Grey.png" );

                BlazarEngine::Physics::PhysicsTransformSystem::setPositionRecursive( child.get( ), glm::vec3( randomRange( ), 0.10, randomRange( ) ) );

                rocks->addChild( std::move( child ) );
            }
        }

        closedir( dir );
    }*/

    tree1 = world->getAssetManager( )->createEntity( PATH( "/assets/models/Tree Type1 04.gltf" ) );

    for ( auto &child: tree1->getChildren( ) )
    {
        auto & texInfo = child->getComponent< BlazarEngine::ECS::CMaterial >( )->textures.emplace_back( BlazarEngine::ECS::Material::TextureInfo { } );
        texInfo.path = PATH( "/assets/textures/Colorsheet Tree Normal.png" );
    }

    tree2 = world->getAssetManager( )->createEntity( PATH( "/assets/models/Tree Type3 04.gltf" ) );

    for ( auto &child: tree2->getChildren( ) )
    {
        auto & texInfo = child->getComponent< BlazarEngine::ECS::CMaterial >( )->textures.emplace_back( BlazarEngine::ECS::Material::TextureInfo { } );
        texInfo.path = PATH( "/assets/textures/Colorsheet Tree Normal.png" );
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
            auto instanceTransform = std::make_unique< ECS::CTransform >( );

            instanceTransform->position = glm::vec3( randomRange( ), 0.15, randomRange( ) );
            instanceTransform->scale = glm::vec3( 0.5f, 0.5f, 0.5f );

            BlazarEngine::Physics::PhysicsTransformSystem::addInstanceRecursive( ptr.get( ), instanceTransform.get( ) );
        }
    }

    initialScene = std::make_unique< Scene::Scene >( );
    initialScene->addEntity( sceneLights.get() );
    initialScene->addEntity( cameraComponent.get() );
    initialScene->addEntity( car1.get() );
    initialScene->addEntity( car2.get() );
    initialScene->addEntity( tree1.get() );
    initialScene->addEntity( tree2.get() );
    initialScene->addEntity( floor.get() );
    initialScene->addEntity( crate.get() );
    initialScene->addEntity( rocks.get() );
    initialScene->addEntity( smallCrate.get() );
    initialScene->addEntity( sky.get() );
    initialScene->addEntity( animDummy.get() );
    world->setScene( initialScene.get() );

    Input::Events::subscribe<Input::WindowResizedParameters * >( Input::EventType::WindowResized, [ & ]( auto windowParams )
    {
        if ( windowParams->width > 0 && windowParams->height > 0 )
        {
            camera->updateAspectRatio( windowParams->width, windowParams->height );
        }
    } );

    Input::Events::subscribe< Input::TickParameters * >( Input::EventType::Tick, [ & ]( auto tickParams )
    {
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

        world->getTransformSystem( )->translate( smallCrate.get( ), translation );
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
