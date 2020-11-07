#pragma once

#include "../core/Common.h"
#include "DrawDescription.h"

NAMESPACES( SomeVulkan, Graphics )

struct DescriptorSetBinding {
    uint16_t index;
    vk::DescriptorType type;
    vk::DescriptorSetLayoutBinding binding;
    vk::DeviceSize size;
};
/*
struct VertextInputBinding {
    AttributeCode attributeCode;
    VkVertexInputBindingDescription vertexInputDescription;
};*/

class ShaderLayout {
protected:
    vk::VertexInputBindingDescription inputBindingDescription;
    std::vector< vk::VertexInputAttributeDescription > vertexAttributeDescriptions;
    std::vector< DescriptorSetBinding > descriptorSetBindings;

public:
    [[nodiscard]] inline const vk::VertexInputBindingDescription& getInputBindingDescription() const {
        return inputBindingDescription;
    }

    [[nodiscard]] inline const std::vector< vk::VertexInputAttributeDescription >& getVertexAttributeDescriptions() const {
        return vertexAttributeDescriptions;
    }

    [[nodiscard]] inline const std::vector< DescriptorSetBinding >& getDescriptorSetBindings() const {
        return descriptorSetBindings;
    }

    inline const uint8_t getDescriptorCount() const {
        return descriptorSetBindings.size();
    }
};

END_NAMESPACES