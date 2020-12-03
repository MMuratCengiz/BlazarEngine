#pragma once

#include "../Core/Common.h"
#include "IComponent.h"

NAMESPACES( SomeVulkan, ECS )

struct CPointLight : public IComponent {
    alignas( 4 ) float attenuationConstant;
    alignas( 4 ) float attenuationLinear;
    alignas( 4 ) float attenuationQuadratic;
    alignas( 16 ) glm::vec4 position;
    alignas( 16 ) glm::vec4 diffuse;
    alignas( 16 ) glm::vec4 specular;

};

END_NAMESPACES