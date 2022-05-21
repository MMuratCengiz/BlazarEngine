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

#include <BlazarECS/CMaterial.h>
#include <BlazarGraphics/RenderGraph/CommonPasses.h>
#include "SampleGame_Small.h"

namespace Sample
{

void SampleGame_Small::init( )
{
    sceneLights = std::make_unique< ECS::DynamicGameEntity >( );
    sceneLights->createComponent< ECS::CAmbientLight >( );
    sceneLights->getComponent< ECS::CAmbientLight >( )->diffuse = glm::vec3( 0.25f, 0.25f, 0.22f );
    sceneLights->getComponent< ECS::CAmbientLight >( )->power = 0.005f;

    glm::vec3 pos = glm::normalize( glm::vec3( 29.65, 18.5822, -4.01676 ) );
    glm::vec3 front = glm::normalize( glm::vec3( -0.708259,-0.615661,0.345442 ) );

    sceneLights->createComponent< ECS::CDirectionalLight >( );
    sceneLights->getComponent< ECS::CDirectionalLight >( )->diffuse = glm::vec3( 1.0, 1.0f, 0.99f );
    sceneLights->getComponent< ECS::CDirectionalLight >( )->direction = front - pos;
    sceneLights->getComponent< ECS::CDirectionalLight >( )->specular = glm::vec3( 1.0, 1.0f, 0.99f );
    sceneLights->getComponent< ECS::CDirectionalLight >( )->power = 0.35f;

    cameraComponent = std::make_unique< ECS::DynamicGameEntity >( );
    cameraComponent->createComponent< ECS::CCamera >( );
    cameraComponent->getComponent< ECS::CCamera >( )->position = glm::vec3( -0.6f, 1.0f, 5.4f );
    camera = std::make_unique< FpsCamera >( cameraComponent->getComponent< ECS::CCamera >( ) );

    floor = std::make_unique< SampleFloor >( world );
    sky = std::make_unique< SampleCubeMap >( world );

//    animDummy = std::make_unique< SampleAnimatedFox >( world );

    car1 = std::make_unique< SampleCar1 >( world );
    car2 = std::make_unique< SampleCar2 >( world );

    shadowMapPass = Graphics::CommonPasses::createShadowMapPass( );
    gBufferPass = Graphics::CommonPasses::createGBufferPass( );
    lightingPass = Graphics::CommonPasses::createLightingPass( );
    skyBoxPass = Graphics::CommonPasses::createSkyBoxPass( );
    presentPass = Graphics::CommonPasses::createPresentPass( );

    world->getGraphSystem()->addPass( shadowMapPass.get( ) );
    world->getGraphSystem()->addPass( gBufferPass.get( ) );
    world->getGraphSystem()->addPass( lightingPass.get( ) );
    world->getGraphSystem()->addPass( skyBoxPass.get( ) );
    //world->getGraphSystem()->addPass( CommonPasses::createSMAAEdgePass( ) );
    //world->getGraphSystem()->addPass( CommonPasses::createSMAABlendWeightPass( ) );
    //world->getGraphSystem()->addPass( CommonPasses::createSMAANeighborPass( ) );
    world->getGraphSystem()->addPass( presentPass.get( ) );

    initialScene = std::make_unique< Scene::Scene >( );
//    initialScene->addEntity( sceneLights );
    initialScene->addEntity( cameraComponent.get( ) );
//    initialScene->addEntity( floor );
    initialScene->addEntity( sky.get() );
//    initialScene->addEntity( car1 );
//    initialScene->addEntity( car2 );
//    initialScene->addEntity( animDummy );
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
}

void SampleGame_Small::update( )
{
    camera->processKeyboardEvents( world->getGLFWwindow( ) );
    camera->processMouseEvents( world->getGLFWwindow( ) );
}

void SampleGame_Small::dispose( )
{

}

}