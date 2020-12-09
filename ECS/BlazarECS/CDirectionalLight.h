#pragma once

#include <BlazarCore/Common.h>
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

struct CDirectionalLight : public IComponent
{
public:
    float power { };
    glm::vec3 diffuse { };
    glm::vec3 specular { };
    glm::vec3 direction { };

    BLAZAR_COMPONENT( CDirectionalLight )
};

END_NAMESPACES