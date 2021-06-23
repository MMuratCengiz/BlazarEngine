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

#include "../GraphicsCommonIncludes.h"
#include "../IRenderDevice.h"
#include "VulkanSurface.h"
#include "VulkanContext.h"
#include "VulkanPipelineProvider.h"
#include "VulkanRenderPassProvider.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class VulkanSurface;
struct Shader;

class VulkanDevice : public IRenderDevice
{
private:
    const std::unordered_map< std::string, bool > ENABLED_LAYERS {
            { "VK_LAYER_KHRONOS_validation", true },
    };

    const std::vector< const char * > REQUIRED_EXTENSIONS {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_MAINTENANCE1_EXTENSION_NAME
    };

    const std::vector< QueueType > queueTypes = {
            QueueType::Graphics,
            QueueType::Transfer,
            QueueType::Presentation
    };

    VkDebugUtilsMessengerEXT debugMessenger { };
    std::unordered_map< std::string, bool > supportedExtensions;
    std::unordered_map< std::string, bool > supportedLayers;

    std::unique_ptr< VulkanContext > context;
    std::unique_ptr< VulkanSurface > renderSurface;
    std::unique_ptr< IPipelineProvider > pipelineProvider;
    std::unique_ptr< IRenderPassProvider > renderPassProvider;
    std::unique_ptr< IResourceProvider > resourceProvider;
public:
    VulkanDevice( ) = default;
    void createDevice( RenderWindow* window ) override;

    std::vector< SelectableDevice > listDevices( ) override;
    void selectDevice( const vk::PhysicalDevice & device );

    void beforeDelete( );

    const std::unique_ptr< IPipelineProvider >& getPipelineProvider( ) const override;
    const std::unique_ptr< IRenderPassProvider >& getRenderPassProvider( ) const override;
    const std::unique_ptr< IResourceProvider >& getResourceProvider( ) const override;
    uint32_t getFrameCount( ) const override;

    const VulkanContext * getContext( ) const;
    ~VulkanDevice( ) override;
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
    void createImageFormat( );
    void initializeVMA( );

    static std::unordered_map< std::string, bool > defaultRequiredExtensions( );
    static void createDeviceInfo( const vk::PhysicalDevice &physicalDevice, DeviceInfo &deviceInfo );
    std::vector< vk::DeviceQueueCreateInfo > createUniqueDeviceCreateInfos( );

    void destroyDebugUtils( ) const;
};

END_NAMESPACES

