#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class AnimationStateSystem : ECS::ISystem
{
    void frameStart( const std::shared_ptr< ECS::ComponentTable > &componentTable ) override;

    void entityTick( const std::shared_ptr< ECS::IGameEntity > &entity ) override;

    void frameEnd( const std::shared_ptr< ECS::ComponentTable > &componentTable ) override;

    void cleanup( ) override;

};

END_NAMESPACES