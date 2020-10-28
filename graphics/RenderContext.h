#pragma once

#include "../core/Common.h"

NAMESPACES( SomeVulkan, Graphics )

typedef struct DeviceInfo {
    VkPhysicalDevice device;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;

    std::vector< VkExtensionProperties > extensionProperties;
    std::vector< VkQueueFamilyProperties > queueFamilies;
} DeviceInfo;

typedef struct QueueFamily {
    uint32_t index;
    VkQueueFamilyProperties properties;
} QueueFamily;


typedef enum class QueueType {
    Graphics,
    Presentation,
} QueueType;

class RenderContext {
public:
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;
    VkPipeline pipeline;
    VkSwapchainKHR swapChain;
    VkSurfaceKHR renderSurface;
    std::vector< VkFramebuffer > frameBuffers;
    VkRenderPass renderPass;
    VkSurfaceKHR surface;
    std::vector< VkImage > images;
    std::vector< VkImageView > imageViews;
    VkExtent2D surfaceExtent{ };
    VkFormat imageFormat;
    VkViewport viewport { };

    GLFWwindow *window;
    std::unordered_map< QueueType, QueueFamily > queueFamilies;
    std::unordered_map< QueueType, VkQueue > queues;
};

END_NAMESPACES