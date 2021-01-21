#pragma once

#include <BlazarCore/Common.h>
#include "CTransform.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

struct COutlined : public IComponent
{
public:
    glm::vec4 outlineColor = glm::vec4( 1.0f );
    float borderScale = 1.1f; // Must be higher than 1

    BLAZAR_COMPONENT( COutlined )
};

END_NAMESPACES
