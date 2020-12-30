#pragma once

#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

struct CCamera : public IComponent
{
public:
    bool isActive = true;
    glm::mat4 view { };
    glm::mat4 projection { };
    glm::vec3 position { };

    BLAZAR_COMPONENT( CCamera )
};

END_NAMESPACES