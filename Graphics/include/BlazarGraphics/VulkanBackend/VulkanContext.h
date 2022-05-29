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
#include <BlazarECS/ECS.h>
#include "../GraphicsCommonIncludes.h"
#include "../RenderWindow.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct VulkanDeviceInfo
{
    vk::PhysicalDevice device;
    vk::PhysicalDeviceProperties properties;
    vk::PhysicalDeviceFeatures features;

    std::vector< vk::ExtensionProperties > extensionProperties;
    std::vector< vk::QueueFamilyProperties > queueFamilies;
};

struct QueueFamily
{
    uint32_t index;
    VkQueueFamilyProperties properties;
};

enum class QueueType
{
    Graphics,
    Presentation,
    Transfer,
};

struct VulkanContext
{
public:
    vk::Instance instance;
    vk::PhysicalDevice physicalDevice;
    vk::Device logicalDevice;
    vma::Allocator vma;
    vk::Format imageFormat;
    vk::ColorSpaceKHR colorSpace;
    vk::PresentModeKHR presentMode;
    vk::SwapchainKHR swapChain;
    vk::SurfaceKHR renderSurface;
    vk::SurfaceKHR surface;
    std::vector< vk::Image > swapChainImages;
    std::vector< vk::ImageView > swapChainImageViews;
    vk::Image depthImage;

    vk::CommandPool transferQueueCommandPool;
    vk::CommandPool graphicsQueueCommandPool;
    vk::CommandPool computeQueueCommandPool;

    vk::Extent2D surfaceExtent { };

    RenderWindow* window;
    std::unordered_map< QueueType, QueueFamily > queueFamilies;
    std::unordered_map< QueueType, vk::Queue > queues;
};

END_NAMESPACES