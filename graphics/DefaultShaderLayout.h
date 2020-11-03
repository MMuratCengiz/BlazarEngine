#pragma once

#include "../core/Common.h"
#include "../graphics/ShaderLayout.h"
#include <glm/glm.hpp>

NAMESPACES( SomeVulkan, Graphics )

typedef struct MVP {
    glm::mat4x4 model;
    glm::mat4x4 view;
    glm::mat4x4 projection;

    [[nodiscard]] uint32_t size() const {
        return 3 /*Matrices*/ * 4 /*columns*/ * 4 /*rows*/;
    }

    static uint32_t fullSize() {
        return 3 /*Matrices*/ * 4 /*columns*/ * 4 /*rows*/ * sizeof( float );
    }

    [[nodiscard]] const MVP* data() const {
        return this;
    }
} MVP;

class DefaultShaderLayout : public ShaderLayout {
private:
    const uint32_t ALL_ELEMENTS_COUNT = 5;

    VkVertexInputAttributeDescription v1Desc = {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = 0
    };

    VkVertexInputAttributeDescription v2Desc = {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = 3 * sizeof( float )
    };

    VkDescriptorSetLayoutBinding uniformDesc {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
    };

    VkDescriptorSetLayoutBinding textureDesc {
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
    };

public:
    DefaultShaderLayout() {
        inputBindingDescription = {
                .binding = 0,
                .stride = 5 * sizeof( float ),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        };

        vertexAttributeDescriptions = {
                v1Desc, v2Desc
        };

        descriptorSetBindings = {
                { .index = 0, .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .binding = uniformDesc, .size = MVP::fullSize() },
                { .index = 1, .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .binding = textureDesc },
        };
    };
};

END_NAMESPACES