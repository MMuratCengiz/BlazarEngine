#pragma once

#include "../core/Common.h"

NAMESPACES(SomeVulkan, Graphics)

class RenderUtilities {
private:
    RenderUtilities() = default;
public:
    static void copyToDeviceMemory(
            const VkDevice& device,
            const VkDeviceMemory& deviceMemory,
            void * target,
            void * data,
            VkDeviceSize size,
            uint32_t sourceOffset = 0,
            uint32_t targetOffset = 0 ) {
        vkMapMemory( device, deviceMemory, sourceOffset, size, targetOffset, &target );
        std::memcpy( target, data, size );
        vkUnmapMemory( device, deviceMemory );
    }
};

END_NAMESPACES