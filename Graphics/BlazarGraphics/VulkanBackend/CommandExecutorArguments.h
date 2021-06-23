/*
Blazar Engine - 3D Game Engine
Copyright (c) 2020-2021 Muhammed Murat Cengiz

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <BlazarCore/Common.h>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

typedef struct CopyBufferToImageArgs
{
    uint32_t width;
    uint32_t height;
    vk::Buffer sourceBuffer;
    vk::Image image;
    uint32_t mipLevel = 0;
    uint32_t arrayLayer = 0;
} CopyBufferToImageArgs;

typedef struct PipelineBarrierArgs
{
    vk::Image image { };
    vk::ImageLayout oldLayout;
    vk::ImageLayout newLayout;
    uint32_t baseMipLevel = 0;
    uint32_t mipLevel = 1;
    uint32_t baseArrayLayer = 0;
    uint32_t layerCount = 1;
    vk::AccessFlags sourceAccess { };
    vk::AccessFlags destinationAccess { };
    vk::PipelineStageFlags sourceStage = vk::PipelineStageFlagBits::eHost;
    vk::PipelineStageFlags destinationStage = vk::PipelineStageFlagBits::eVertexShader;
} PipelineBarrierArgs;

typedef struct ImageBlitArgs
{
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