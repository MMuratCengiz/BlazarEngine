#pragma once

#include <unordered_map>
#include <vulkan/vulkan.h>
#include "../core/Common.h"
#include "RenderSurface.h"
#include "RenderDeviceBuilder.h"
#include "RenderContext.h"

/*
 *
 * Desired usage:
 *
 * auto vulkanAPI { window };
 * vulkanAPI.selectDevice( GPU );
 * auto pipeline = vulkanAPI.createPipeline( shaders );
 * pipeline.setGlobal<T>( Type t, T value );
 * pipeline.addRenderObject( renderObject );
 * pipeline.render();
 */

NAMESPACES( SomeVulkan, Graphics )

class RenderSurface;
struct Shader;

namespace T_FUNC {
typedef const std::function< bool( const DeviceInfo &pDevice ) > &deviceCapabilityCheck;
typedef const std::function< bool( QueueType index ) > &findQueueType;
}

class RenderDevice {
public:
    const std::unordered_map< QueueType, uint32_t > QUEUE_TYPE_FLAGS = {
            { QueueType::Graphics, VK_QUEUE_GRAPHICS_BIT },
    };
private:
    const std::unordered_map< std::string, bool > ENABLED_LAYERS {
            { "VK_LAYER_KHRONOS_validation", true },
    };

    const std::vector< const char * > REQUIRED_EXTENSIONS {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    const std::vector< QueueType > queueTypes = {
            QueueType::Graphics,
            QueueType::Presentation
    };

    VkDebugUtilsMessengerEXT debugMessenger { };
    std::unordered_map< std::string, bool > supportedExtensions;
    std::unordered_map< std::string, bool > supportedLayers;

    bool queueFamiliesPrepared = false;
    bool logicalDeviceCreated = false;

    std::shared_ptr< RenderContext > context;
public:
    explicit RenderDevice( GLFWwindow *window );

    std::vector< DeviceInfo > listGPUs( T_FUNC::deviceCapabilityCheck onlyValidDevices = defaultDeviceCapabilityCheck );
    void selectDevice( const DeviceInfo& deviceInfo );

    void beforeDelete();

    std::shared_ptr< RenderContext > getContext() const;
    std::unique_ptr< RenderSurface > createRenderSurface( const std::vector< Shader >& shaders );

    ~RenderDevice( );
private:
    void initSupportedExtensions( );
    void initDebugMessages( const VkDebugUtilsMessengerCreateInfoEXT &createInfo );
    void initSupportedLayers( std::vector< const char * > &layers );

    VkDebugUtilsMessengerCreateInfoEXT getDebugUtilsCreateInfo( ) const;
    void setupQueueFamilies( );
    void createLogicalDevice( );
    void createSurface( );

    void addQueueFamily( uint32_t index, const VkQueueFamilyProperties &properties, T_FUNC::findQueueType &exists );

    static std::unordered_map< std::string, bool > defaultRequiredExtensions();
    static bool defaultDeviceCapabilityCheck( const DeviceInfo& deviceInfo );
    static void createDeviceInfo( const VkPhysicalDevice &physicalDevice, DeviceInfo& deviceInfo );
    std::vector< VkDeviceQueueCreateInfo > createUniqueDeviceCreateInfos( );

    friend class RenderDeviceBuilder;
};

END_NAMESPACES

