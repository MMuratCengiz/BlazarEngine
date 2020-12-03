#pragma once

#include "../Core/Common.h"
#include "IComponent.h"

NAMESPACES( SomeVulkan, ECS )

struct CAmbientLight : public IComponent {
    alignas( 16 ) glm::vec4 diffuse;
    alignas( 16 ) glm::vec4 specular;
    alignas( 4 ) float power;
};


END_NAMESPACES