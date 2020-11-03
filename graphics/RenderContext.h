#pragma once

#include "../core/Common.h"
#include "../graphics/DescriptorManager.h"

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
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout{};
    VkSwapchainKHR swapChain;
    VkSurfaceKHR renderSurface;
    std::vector< VkFramebuffer > frameBuffers;
    VkRenderPass renderPass;
    VkSurfaceKHR surface;
    std::vector< VkImage > swapChainImages;
    std::vector< VkImageView > imageViews;
    VkExtent2D surfaceExtent { };
    VkFormat imageFormat;
    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout{ };
    VkViewport viewport { };
    std::vector< VkDescriptorSet > descriptorSets{ };
    std::shared_ptr< DescriptorManager > descriptorManager; // todo maybe move everything related to descriptors?

    GLFWwindow *window;
    std::unordered_map< QueueType, QueueFamily > queueFamilies;
    std::unordered_map< QueueType, VkQueue > queues;


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