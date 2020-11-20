#pragma once

#include "../core/Common.h"
#include "IComponent.h"

NAMESPACES( SomeVulkan, ECS )

struct CTransform: public IComponent {
    glm::mat4x4 model;
};


END_NAMESPACES