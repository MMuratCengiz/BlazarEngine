#pragma once

#include "../core/Common.h"

NAMESPACES( SomeVulkan, Graphics)

struct ObjectBufferPart {
    uint64_t vertexCount;
    uint64_t indexCount;

    std::pair< vk::Buffer, vma::Allocation > vertexBuffer;
    std::pair< vk::Buffer, vma::Allocation > indexBuffer;
};

struct ObjectBuffer {
    std::vector< ObjectBufferPart > parts;
};

END_NAMESPACES
