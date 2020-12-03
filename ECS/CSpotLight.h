#pragma once

#include "../Core/Common.h"
#include "IComponent.h"

NAMESPACES( SomeVulkan, ECS )

struct CSpotLight : public IComponent {
    alignas( 4 ) float power;
    alignas( 4 ) float radius;
    alignas( 16 ) glm::vec4 position;
    alignas( 16 ) glm::vec4 direction;
    alignas( 16 ) glm::vec4 diffuse;
    alignas( 16 ) glm::vec4 specular;
};

END_NAMESPACES