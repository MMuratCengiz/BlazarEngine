#pragma once

#include "../core/Common.h"

NAMESPACES( SomeVulkan, Scene )

class Camera {
public:
    virtual glm::mat4 getView() = 0;
    virtual glm::mat4 getProjection() = 0;
    virtual glm::vec3 getPosition() = 0;
};

END_NAMESPACES