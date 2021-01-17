#pragma once

#include <BlazarCore/Common.h>
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

struct CTessellation : public IComponent
{
public:
    float innerLevel = 0.0f;
    float outerLevel = 0.0f;

    BLAZAR_COMPONENT( CTessellation )
};

END_NAMESPACES