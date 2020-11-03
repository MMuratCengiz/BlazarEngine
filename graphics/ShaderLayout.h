#pragma once

#include "../core/Common.h"
#include "DrawDescription.h"

NAMESPACES( SomeVulkan, Graphics )

struct DescriptorSetBinding {
    uint16_t index;
    VkDescriptorType type;
    VkDescriptorSetLayoutBinding binding;
    VkDeviceSize size;
};
/*
struct VertextInputBinding {
    AttributeCode attributeCode;
    VkVertexInputBindingDescription vertexInputDescription;
};*/

class ShaderLayout {
protected:
    VkVertexInputBindingDescription inputBindingDescription;
    std::vector< VkVertexInputAttributeDescription > vertexAttributeDescriptions;
    std::vector< DescriptorSetBinding > descriptorSetBindings;

public:
    [[nodiscard]] inline const VkVertexInputBindingDescription& getInputBindingDescription() const {
        return inputBindingDescription;
    }

    [[nodiscard]] inline const std::vector< VkVertexInputAttributeDescription >& getVertexAttributeDescriptions() const {
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