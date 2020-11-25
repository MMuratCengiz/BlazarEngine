#pragma once

#include "../Core/Common.h"
#include <unordered_map>
#include <vulkan/vulkan.hpp>
#include "RenderSurface.h"
#include "RenderDeviceBuilder.h"
#include "InstanceContext.h"
#include "../Scene/FpsCamera.h"

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
public:
    explicit RenderDevice( GLFWwindow *window );

    std::vector< DeviceInfo > listGPUs( T_FUNC::deviceCapabilityCheck onlyValidDevices = defaultDeviceCapabilityCheck );
    void selectDevice( const DeviceInfo& deviceInfo );

    void beforeDelete();

    std::shared_ptr< InstanceContext > getContext() const;
    std::unique_ptr< RenderSurface > createRenderSurface( const std::vector< ShaderInfo >& shaders, const std::shared_ptr< Scene::Camera >& camera );

    ~RenderDevice( );
private:
    void initSupportedExtensions( );
    void initDebugMessages( const vk::DebugUtilsMessengerCreateInfoEXT &createInfo );
    void initSupportedLayers( std::vector< const char * > &layers );

    vk::DebugUtilsMessengerCreateInfoEXT getDebugUtilsCreateInfo( ) const;
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

