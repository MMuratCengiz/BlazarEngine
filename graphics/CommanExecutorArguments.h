#pragma once

#include "../core/Common.h"

NAMESPACES( SomeVulkan, Graphics )

typedef struct CopyBufferToImageArgs {
    uint32_t width;
    uint32_t height;
    VkBuffer sourceBuffer;
    VkImage image;
} CopyBufferToImageArgs;

typedef struct PipelineBarrierArgs {
    VkImage image { };
    VkImageLayout oldLayout;
    VkImageLayout newLayout;
    uint32_t baseMipLevel = 0;
    uint32_t mipLevel = 1;
    VkAccessFlags sourceAccess { };
    VkAccessFlags destinationAccess { };
    VkShaderStageFlags sourceStage = VK_PIPELINE_STAGE_HOST_BIT;
    VkShaderStageFlags destinationStage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
} PipelineBarrierArgs;

typedef struct ImageBlitArgs {
    VkImage sourceImage;
    VkImageLayout sourceImageLayout;
    VkImage destinationImage;
    VkImageLayout destinationImageLayout;
    VkImageSubresourceLayers srcSubresource;
    VkOffset3D srcOffsets[2];
    VkImageSubresourceLayers dstSubresource;
    VkOffset3D dstOffsets[2];
} ImageBlitArgs;

END_NAMESPACES