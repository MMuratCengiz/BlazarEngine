#pragma once

#include "../core/Common.h"

NAMESPACES( SomeVulkan, Graphics )

typedef struct CopyBufferToImageArgs {
    uint32_t width;
    uint32_t height;
    vk::Buffer sourceBuffer;
    vk::Image image;
} CopyBufferToImageArgs;

typedef struct PipelineBarrierArgs {
    vk::Image image { };
    vk::ImageLayout oldLayout;
    vk::ImageLayout newLayout;
    uint32_t baseMipLevel = 0;
    uint32_t mipLevel = 1;
    vk::AccessFlags sourceAccess { };
    vk::AccessFlags destinationAccess { };
    vk::PipelineStageFlags sourceStage = vk::PipelineStageFlagBits::eHost;
    vk::PipelineStageFlags destinationStage = vk::PipelineStageFlagBits::eVertexShader;
} PipelineBarrierArgs;

typedef struct ImageBlitArgs {
    vk::Image sourceImage;
    vk::ImageLayout sourceImageLayout;
    vk::Image destinationImage;
    vk::ImageLayout destinationImageLayout;
    vk::ImageSubresourceLayers srcSubresource;
    vk::Offset3D srcOffsets[2];
    vk::ImageSubresourceLayers dstSubresource;
    vk::Offset3D dstOffsets[2];
} ImageBlitArgs;

END_NAMESPACES