#pragma once

#include "../Core/Common.h"
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

struct CAmbientLight : public IComponent
{
    float power;
    glm::vec3 diffuse;
    glm::vec3 specular;
};


END_NAMESPACES