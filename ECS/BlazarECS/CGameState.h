#pragma once

#include <BlazarCore/Common.h>
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

struct CGameState : public IComponent
{
public:
    uint32_t surfaceWidth = 0.0f;
    uint32_t surfaceHeight = 0.0f;

    BLAZAR_COMPONENT( CGameState )
};

END_NAMESPACES