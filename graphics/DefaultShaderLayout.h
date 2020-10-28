#pragma once

#include "../core/Common.h"
#include <glm/glm.hpp>

NAMESPACES( SomeVulkan, Graphics )

class DefaultShaderLayout {
private:
    static const uint32_t ALL_ELEMENTS_COUNT;

    glm::vec3 pos;
    glm::vec3 color;

    VkVertexInputBindingDescription description = {
        .binding = 0,
        .stride = ALL_ELEMENTS_COUNT * sizeof( float ),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    VkVertexInputAttributeDescription v1Desc = {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT
    };

    VkVertexInputAttributeDescription v2Desc = {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = 3 * sizeof( float )
    };

    std::array< VkVertexInputAttributeDescription, 2 > descriptions = { v1Desc, v2Desc };

public:
    DefaultShaderLayout() = default;
    DefaultShaderLayout( glm::vec3 pos, glm::vec3 color );
    const VkVertexInputBindingDescription& getBindingDescription( );
    const std::array< VkVertexInputAttributeDescription, 2 >& getAttributeDescription( );
};

END_NAMESPACES