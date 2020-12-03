#pragma once

#include "../Core/Common.h"

NAMESPACES( SomeVulkan, Graphics )

struct DeviceInfo {
    vk::PhysicalDevice device;
    vk::PhysicalDeviceProperties properties;
    vk::PhysicalDeviceFeatures features;

    std::vector< vk::ExtensionProperties > extensionProperties;
    std::vector< vk::QueueFamilyProperties > queueFamilies;
};

struct QueueFamily {
    uint32_t index;
    VkQueueFamilyProperties properties;
};

enum class QueueType {
    Graphics,
    Presentation,
    Transfer,
};

struct InstanceContext {
public:
    vk::Instance instance;
    vk::PhysicalDevice physicalDevice;
    vk::Device logicalDevice;
    vma::Allocator vma;
    vk::Format imageFormat;
    vk::ColorSpaceKHR colorSpace;
    vk::PresentModeKHR presentMode;
    vk::RenderPass renderPass;
    vk::SwapchainKHR swapChain;
    vk::SurfaceKHR renderSurface;
    std::vector< vk::Framebuffer > frameBuffers;
    vk::SurfaceKHR surface;
    std::vector< vk::Image > swapChainImages;
    std::vector< vk::ImageView > imageViews;
    vk::Image depthImage;
    vk::DeviceMemory depthMemory;
    vk::ImageView depthView;
    vk::Extent2D surfaceExtent { };
    vk::Viewport viewport { };
    vk::Rect2D viewScissor { };

    GLFWwindow *window;
    std::unordered_map< QueueType, QueueFamily > queueFamilies;
    std::unordered_map< QueueType, vk::Queue > queues;
};

typedef std::shared_ptr< InstanceContext > pInstanceContext;

END_NAMESPACES