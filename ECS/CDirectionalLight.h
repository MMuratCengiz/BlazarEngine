#pragma once

#include "../Core/Common.h"
#include "IComponent.h"

NAMESPACES( SomeVulkan, ECS )

struct CDirectionalLight : public IComponent {
    float power;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 direction;
};

END_NAMESPACES