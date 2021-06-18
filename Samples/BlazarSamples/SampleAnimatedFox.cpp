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

    if ( animState != nullptr )
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
    animState->currentNode->transitions[ 2 ] = swordAndShieldJump;
    animState->currentNode->transitions[ 3 ] = run;
    animState->currentNode->transitions[ 4 ] = attack;
    animState->state = 1;

    Input::ActionBinding animChange { };
    animChange.keyCode = Input::KeyboardKeyCode::T;
    animChange.pressForm = Input::KeyPressForm::Pressed;
    animChange.controller = Input::Controller::Keyboard;

    world->getActionMap( )->registerAction( "ChangeAnim", animChange );

    auto changeAnim = [ = ]( const std::string &actionName )
    {
        auto animState = entity->getComponent< ECS::CAnimState >( )->state++;
        if ( entity->getComponent< ECS::CAnimState >( )->state > 4 )
        {
            entity->getComponent< ECS::CAnimState >( )->state = 1;
        }
    };

    world->getActionMap( )->subscribeToAction( "ChangeAnim", changeAnim );
}

}