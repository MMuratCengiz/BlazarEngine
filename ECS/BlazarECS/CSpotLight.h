#pragma once

#include <BlazarCore/Common.h>
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

struct CSpotLight : public IComponent
{
public:
    float power { };
    float radius { };
    glm::vec3 position { };
    glm::vec3 direction { };
    glm::vec3 diffuse { };
    glm::vec3 specular { };

    BLAZAR_COMPONENT( CSpotLight )
};

END_NAMESPACES