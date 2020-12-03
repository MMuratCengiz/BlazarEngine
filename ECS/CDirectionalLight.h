#pragma once

#include "../Core/Common.h"
#include "IComponent.h"

NAMESPACES( SomeVulkan, ECS )

struct CDirectionalLight : public IComponent {
    alignas( 4 ) float power;
    alignas( 16 ) glm::vec4 diffuse;
    alignas( 16 ) glm::vec4 specular;
    alignas( 16 ) glm::vec4 direction;
};

END_NAMESPACES