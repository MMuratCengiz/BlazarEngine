#pragma once

#include "../Core/Common.h"
#include <unordered_map>
#include "RenderSurface.h"
#include "RenderDeviceBuilder.h"
#include "InstanceContext.h"
#include "../Scene/FpsCamera.h"

NAMESPACES( SomeVulkan, Graphics )

class RenderSurface;
struct Shader;

namespace T_FUNC {
typedef const std::function< bool( const DeviceInfo &pDevice ) > &deviceCapabilityCheck;
typedef const std::function< bool( QueueType index ) > &findQueueType;
}

class RenderDevice {
private:
    const std::unordered_map< std::string, bool > ENABLED_LAYERS {
            { "VK_LAYER_KHRONOS_validation", true },
    };

    const std::vector< const char * > REQUIRED_EXTENSIONS {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    const std::vector< QueueType > queueTypes = {
            QueueType::Graphics,
            QueueType::Transfer,
            QueueType::Presentation
    };

    VkDebugUtilsMessengerEXT debugMessenger { };
    std::unordered_map< std::string, bool > supportedExtensions;
    std::unordered_map< std::string, bool > supportedLayers;

    std::shared_ptr< InstanceContext > context;
    std::unique_ptr< RenderSurface > renderSurface;
public:
    explicit RenderDevice( GLFWwindow *window );

    std::vector< DeviceInfo > listGPUs( T_FUNC::deviceCapabilityCheck onlyValidDevices = defaultDeviceCapabilityCheck );
    void selectDevice( const DeviceInfo& deviceInfo );

    void beforeDelete();

    std::shared_ptr< InstanceContext > getContext() const;
    const std::shared_ptr< Renderer >& getRenderer();
    ~RenderDevice( );
private:
    void createRenderSurface( );

    void initSupportedExtensions( );
    void initDebugMessages( const vk::DebugUtilsMessengerCreateInfoEXT &createInfo );
    void initSupportedLayers( std::vector< const char * > &layers );

    vk::DebugUtilsMessengerCreateInfoEXT getDebugUtilsCreateInfo( ) const;
    static void loadExtensionFunctions( );
    void setupQueueFamilies( );
    void createLogicalDevice( );
    void createSurface( );
    void createRenderPass( );
    void initializeVMA( );

    static std::unordered_map< std::string, bool > defaultRequiredExtensions();
    static bool defaultDeviceCapabilityCheck( const DeviceInfo& deviceInfo );
    static void createDeviceInfo( const vk::PhysicalDevice &physicalDevice, DeviceInfo& deviceInfo );
    std::vector< vk::DeviceQueueCreateInfo > createUniqueDeviceCreateInfos( );

    friend class RenderDeviceBuilder;
    void destroyDebugUtils( ) const;
};

END_NAMESPACES

