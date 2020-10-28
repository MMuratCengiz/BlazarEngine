#pragma once

#include "../core/Common.h"

NAMESPACES(SomeVulkan, Graphics)

struct VertexDescriptor {
    bool indexedMode = false;

    std::vector< float > vertices;
    std::vector< glm::vec3 > normals;
    std::vector< uint32_t > indices;
};

END_NAMESPACES
