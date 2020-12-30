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
    vk::Viewport viewport { };
    vk::Rect2D viewScissor { };

    RenderWindow* window;
    std::unordered_map< QueueType, QueueFamily > queueFamilies;
    std::unordered_map< QueueType, vk::Queue > queues;
};

END_NAMESPACES