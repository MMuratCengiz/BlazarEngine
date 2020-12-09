#pragma once

#include <BlazarCore/Common.h>
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

struct CAmbientLight : public IComponent
{
public:
    float power { };
    glm::vec3 diffuse { };
    glm::vec3 specular{ };

    BLAZAR_COMPONENT( CAmbientLight )
};


END_NAMESPACES