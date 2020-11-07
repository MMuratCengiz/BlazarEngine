#pragma once

#include "../core/Common.h"
#include "../graphics/DescriptorManager.h"

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
};

typedef enum class EventType {
    SwapChainInvalidated
} EventType;

class RenderContext;
class DescriptorManager;

namespace FunctionDefinitions {
typedef std::function< void( RenderContext *context, EventType eventType ) > eventCallback;
}

typedef std::shared_ptr< RenderContext > pRenderContext;
/*
 * TODO
 * Split this into DeviceContext and Pipeline context, because we may have different pipelines for every device,
 * ie. different shaders
 */

class RenderContext {
private:
    std::unordered_map< EventType, std::vector< FunctionDefinitions::eventCallback > > eventSubscribers;
public:
    vk::Instance instance;
    vk::PhysicalDevice physicalDevice;
    vk::Device logicalDevice;
    vk::Pipeline pipeline;
    vk::PipelineLayout pipelineLayout{};
    vk::SwapchainKHR swapChain;
    vk::SurfaceKHR renderSurface;
    std::vector< vk::Framebuffer > frameBuffers;
    vk::RenderPass renderPass;
    vk::SurfaceKHR surface;
    std::vector< vk::Image > swapChainImages;
    std::vector< vk::ImageView > imageViews;
    vk::Image depthImage;
    vk::DeviceMemory depthMemory;
    vk::ImageView depthView;
    vk::Extent2D surfaceExtent { };
    vk::Format imageFormat;
    vk::DescriptorPool descriptorPool;
    vk::DescriptorSetLayout descriptorSetLayout{ };
    vk::Viewport viewport { };
    std::vector< vk::DescriptorSet > descriptorSets{ };
    std::shared_ptr< DescriptorManager > descriptorManager; // todo maybe move everything related to descriptors?

    GLFWwindow *window;
    std::unordered_map< QueueType, QueueFamily > queueFamilies;
    std::unordered_map< QueueType, vk::Queue > queues;


    // Todo move these somewhere else, maybe global event handlers?
    void subscribeToEvent( EventType event, const FunctionDefinitions::eventCallback& cb ) {
        ensureMapContainsEvent( event );
        eventSubscribers[ event ].emplace_back( cb );
    }

    // Todo move these somewhere else
    void triggerEvent( EventType event ) {
        ensureMapContainsEvent( event );

        for ( const auto& cb: eventSubscribers[ event ] ) {
            cb( this, event );
        }
    }

private:
    void ensureMapContainsEvent( EventType event ) {
        if ( eventSubscribers.find( event ) == eventSubscribers.end( ) ) {
            eventSubscribers[ event ] = { };
        }
    }
};

END_NAMESPACES