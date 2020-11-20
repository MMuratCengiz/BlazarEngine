#pragma once

#include "../core/Common.h"
#include "IComponent.h"

NAMESPACES( SomeVulkan, ECS )

struct CTransform: public IComponent {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};


END_NAMESPACES