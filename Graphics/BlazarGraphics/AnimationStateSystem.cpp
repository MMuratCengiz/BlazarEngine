//
// Created by Murat on 6/12/2021.
//

#include "AnimationStateSystem.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

void AnimationStateSystem::frameStart( const std::shared_ptr< ECS::ComponentTable > &componentTable )
{
    for ( std::shared_ptr< ECS::CAnimState > animState: componentTable->getComponents< ECS::CAnimState >( ) )
    {

    }
}

void AnimationStateSystem::entityTick( const std::shared_ptr< ECS::IGameEntity > &entity ) {  }

void AnimationStateSystem::frameEnd( const std::shared_ptr< ECS::ComponentTable > &componentTable )
{

}

void AnimationStateSystem::cleanup( )
{

}

END_NAMESPACES