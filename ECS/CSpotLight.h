#pragma once

#include "../Core/Common.h"
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

struct CSpotLight : public IComponent
{
    float power;
    float radius;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

END_NAMESPACES