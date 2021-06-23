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

#include "SampleAnimatedFox.h"

using namespace BlazarEngine;

namespace Sample
{

SampleAnimatedFox::SampleAnimatedFox( Scene::World *world )
{
    this->world = world;

    auto meshEntities = world->getAssetManager( )->createEntity( PATH( "/assets/models/scene.gltf" ) );

    addChild( meshEntities );

    BlazarEngine::Physics::PhysicsTransformSystem::setPositionRecursive( this, glm::vec3( 3.0f, 0.15f, 5.0f ) );
    BlazarEngine::Physics::PhysicsTransformSystem::setScaleRecursive( this, glm::vec3( 1.0f, 1.0f, 1.0f ) );

    iterChildren( meshEntities );
}

void SampleAnimatedFox::iterChildren( const std::shared_ptr< ECS::IGameEntity >& entity )
{
    for ( const auto& child: entity->getChildren( ) )
    {
        iterChildren( child );
    }

    auto animState = entity->getComponent< ECS::CAnimState >( );

    if ( animState == nullptr )
    {
        return;
    }

    auto * swordAndShieldJump = new ECS::CAnimFlowNode { };
    swordAndShieldJump->animName = "SwordAndShieldJump";

    auto * surveyTransition = new ECS::CAnimFlowNode { };
    surveyTransition->animName = "Idle";

    auto * run = new ECS::CAnimFlowNode { };
    run->animName = "SwordAndShieldRun";

    auto * attack = new ECS::CAnimFlowNode { };
    attack->animName = "SwordAndShieldSlash";

    animState->currentNode->transitions[ 1 ] = surveyTransition;
    surveyTransition->transitions[ 2 ] = swordAndShieldJump;
    swordAndShieldJump->transitions[ 3 ] = run;
    run->transitions[ 4 ] = attack;
    attack->transitions[ 1 ] = surveyTransition;
    animState->state = 1;

    Input::ActionBinding animChange { };
    animChange.keyCode = Input::KeyboardKeyCode::T;
    animChange.pressForm = Input::KeyPressForm::Pressed;
    animChange.controller = Input::Controller::Keyboard;

    world->getActionMap( )->registerAction( "ChangeAnim", animChange );

    auto changeAnim = [ = ]( const std::string &actionName )
    {
        entity->getComponent< ECS::CAnimState >( )->state
            = entity->getComponent< ECS::CAnimState >( )->currentNode->transitions.begin()->first;
    };

    world->getActionMap( )->subscribeToAction( "ChangeAnim", changeAnim );
}

}