//
// Created by Murat on 10/9/2020.
//

#include "RenderDevice.h"
#include "GraphicsException.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                     VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                     const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                     void *pUserData ) {
    int verbosity;

    switch ( messageSeverity ) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            verbosity = VERBOSITY_LOW;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            verbosity = VERBOSITY_INFORMATION;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            verbosity = VERBOSITY_HIGH;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            verbosity = VERBOSITY_CRITICAL;
            break;
    }

    TRACE( COMPONENT_VKAPI, verbosity, pCallbackData->pMessage )

    return VK_FALSE;
}

SomeVulkan::Graphics::RenderDevice::RenderDevice( GLFWwindow *window ) {
    context = std::make_shared< RenderContext >( );
    context->window = window;

    VkApplicationInfo appInfo {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = "SomeVulkan",
            .applicationVersion = VK_MAKE_VERSION( 1, 0, 0 ),
            .pEngineName = "No Engine",
            .engineVersion = VK_MAKE_VERSION( 1, 0, 0 ),
            .apiVersion = VK_API_VERSION_1_2,
    };

    VkInstanceCreateInfo createInfo {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &appInfo
    };

    uint32_t glfwExtensionCount;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );
    std::vector< const char * > extensions( glfwExtensions, glfwExtensions + glfwExtensionCount );

#if DEBUG
    extensions.emplace_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
#endif
    createInfo.enabledExtensionCount = glfwExtensionCount + 1;
    createInfo.ppEnabledExtensionNames = extensions.data( );

    std::vector< const char * > layers;
    initSupportedLayers( layers );

    VkDebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo = getDebugUtilsCreateInfo( );

#if DEBUG
    createInfo.pNext = ( VkDebugUtilsMessengerCreateInfoEXT * ) &debugUtilsCreateInfo;

    if ( supportedLayers.find( "VK_LAYER_KHRONOS_validation" ) == supportedLayers.end( ) ) {
        createInfo.enabledLayerCount = 0;
        TRACE( COMPONENT_VKAPI, VERBOSITY_INFORMATION, "Layer: VK_LAYER_KHRONOS_validation not found." )
    } else {
        createInfo.enabledLayerCount = static_cast<uint32_t>( layers.size( ));
        createInfo.ppEnabledLayerNames = layers.data( );
    }
#elif
    createInfo.enabledLayerCount = 0;
#endif

    VkResult createInstanceResult = vkCreateInstance( &createInfo, nullptr, &context->instance );

    if ( createInstanceResult != VK_SUCCESS ) {

        std::string es;

        SFORMAT( 1, "VkInstance initialization failed, code: " << createInstanceResult << ".", es );

        TRACE( COMPONENT_VKAPI, VERBOSITY_CRITICAL, es.c_str( ) )

        throw GraphicsException( GraphicsException::Source::RenderDevice, "Call to vkCreateInstance failed!" );
    }

    initSupportedExtensions( );
    initDebugMessages( debugUtilsCreateInfo );

    TRACE( COMPONENT_VKAPI, VERBOSITY_INFORMATION, "All Vk components initialized successfully." )

    createSurface( );
}

void SomeVulkan::Graphics::RenderDevice::initSupportedExtensions( ) {
    uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, nullptr );

    std::vector< VkExtensionProperties > extensionProperties( extensionCount );
    vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, extensionProperties.data( ) );

    for ( VkExtensionProperties prp: extensionProperties ) {
        this->supportedExtensions[ prp.extensionName ] = true;
    }
}

void SomeVulkan::Graphics::RenderDevice::initSupportedLayers( std::vector< const char * > &layers ) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

    std::vector< VkLayerProperties > layerProperties( layerCount );
    vkEnumerateInstanceLayerProperties( &layerCount, layerProperties.data( ) );

    for ( VkLayerProperties prp: layerProperties ) {
        auto layerPair = ENABLED_LAYERS.find( prp.layerName );

        if ( layerPair != ENABLED_LAYERS.end( ) ) {
            supportedLayers[ prp.layerName ] = true;
            layers.emplace_back( layerPair->first.c_str( ) );
        }
    }
}

VkDebugUtilsMessengerCreateInfoEXT SomeVulkan::Graphics::RenderDevice::getDebugUtilsCreateInfo( ) const {
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo { };
    debugUtilsCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugUtilsCreateInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugUtilsCreateInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugUtilsCreateInfo.pfnUserCallback = debugCallback;
    debugUtilsCreateInfo.pUserData = nullptr;
    return debugUtilsCreateInfo;
}

void SomeVulkan::Graphics::RenderDevice::initDebugMessages( const VkDebugUtilsMessengerCreateInfoEXT &createInfo ) {
    auto createDebugUtils = ( PFN_vkCreateDebugUtilsMessengerEXT ) vkGetInstanceProcAddr( context->instance,
                                                                                          "vkCreateDebugUtilsMessengerEXT" );

    if ( createDebugUtils == nullptr ||
         createDebugUtils( context->instance, &createInfo, nullptr, &debugMessenger ) != VK_SUCCESS ) {
        TRACE( COMPONENT_VKAPI, VERBOSITY_HIGH, "Failed to create Vk debugger." )

        throw GraphicsException( GraphicsException::Source::RenderDevice, "Failed to initialize debugger!" );
    }
}

std::vector< SomeVulkan::Graphics::DeviceInfo >
SomeVulkan::Graphics::RenderDevice::listGPUs( T_FUNC::deviceCapabilityCheck deviceCapabilityCheck ) {
    uint32_t deviceCount;

    vkEnumeratePhysicalDevices( context->instance, &deviceCount, nullptr );

    std::vector< VkPhysicalDevice > devices( deviceCount );
    std::vector< DeviceInfo > result;

    vkEnumeratePhysicalDevices( context->instance, &deviceCount, devices.data( ) );

    for ( auto it = devices.begin( ); it != devices.end( ); ) {
        VkPhysicalDevice physicalDevice = *it;

        DeviceInfo deviceInfo {
                .device = physicalDevice
        };

        createDeviceInfo( physicalDevice, deviceInfo );

        if ( !deviceCapabilityCheck( deviceInfo ) ) {
            devices.erase( it );
        } else {
            result.emplace_back( deviceInfo );
            ++it;
        }
    }

    return result;
}

void
SomeVulkan::Graphics::RenderDevice::createDeviceInfo( const VkPhysicalDevice &physicalDevice, DeviceInfo &deviceInfo ) {
    VkPhysicalDeviceFeatures deviceFeatures;
    std::vector< VkQueueFamilyProperties > localQueueFamilies;

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties( physicalDevice, &deviceProperties );
    vkGetPhysicalDeviceFeatures( physicalDevice, &deviceFeatures );

    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &queueFamilyCount, nullptr );
    vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &queueFamilyCount, localQueueFamilies.data( ) );

    uint32_t extensionCount;

    vkEnumerateDeviceExtensionProperties( physicalDevice, nullptr, &extensionCount, nullptr );

    std::vector< VkExtensionProperties > extensions( extensionCount );

    vkEnumerateDeviceExtensionProperties( physicalDevice, nullptr, &extensionCount, extensions.data( ) );

    deviceInfo.properties = deviceProperties;
    deviceInfo.features = deviceFeatures;
    deviceInfo.extensionProperties = extensions;
}

void SomeVulkan::Graphics::RenderDevice::selectDevice( const DeviceInfo &deviceInfo ) {
    context->physicalDevice = deviceInfo.device;

    createLogicalDevice( );
}

void SomeVulkan::Graphics::RenderDevice::setupQueueFamilies( ) {
    if ( !queueFamiliesPrepared ) {
        auto exists = [ & ]( QueueType bit ) -> bool {
            return context->queueFamilies.find( bit ) != context->queueFamilies.end( );
        };

        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties( context->physicalDevice, &queueFamilyCount, nullptr );

        std::vector< VkQueueFamilyProperties > localQueueFamilies( queueFamilyCount );
        vkGetPhysicalDeviceQueueFamilyProperties( context->physicalDevice, &queueFamilyCount,
                                                  localQueueFamilies.data( ) );

        uint32_t index = 0;
        for ( VkQueueFamilyProperties properties: localQueueFamilies ) {
            addQueueFamily( index, properties, exists );

            VkBool32 presentationSupport;
            vkGetPhysicalDeviceSurfaceSupportKHR( context->physicalDevice, index, context->surface,
                                                  &presentationSupport );

            if ( presentationSupport && !exists( QueueType::Presentation ) ) {
                context->queueFamilies[ QueueType::Presentation ] = QueueFamily {
                        .index = index,
                        .properties = properties
                };
            }

            ++index;
        }
    }

    queueFamiliesPrepared = true;
}

void SomeVulkan::Graphics::RenderDevice::addQueueFamily( uint32_t index, const VkQueueFamilyProperties &properties,
                                                         T_FUNC::findQueueType &exists ) {
    for ( QueueType queueType: queueTypes ) {
        if ( QUEUE_TYPE_FLAGS.find( queueType ) != QUEUE_TYPE_FLAGS.end( ) ) {
            uint32_t flag = QUEUE_TYPE_FLAGS.at( queueType );

            if ( properties.queueFlags & flag && !exists( queueType ) ) {
                context->queueFamilies[ queueType ] = QueueFamily { .index = index, .properties = properties };
            }
        }
    }
}

void SomeVulkan::Graphics::RenderDevice::createLogicalDevice( ) {
    setupQueueFamilies( );

    if ( logicalDeviceCreated ) {
        return;
    }

    logicalDeviceCreated = true;

    std::vector< VkDeviceQueueCreateInfo > deviceQueueCreateInfos = createUniqueDeviceCreateInfos( );

    VkPhysicalDeviceFeatures features { };
    features.samplerAnisotropy = true;

#ifdef DEBUG
    std::vector< const char * > layers;
    initSupportedLayers( layers );
#endif
    VkDeviceCreateInfo createInfo {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = static_cast< uint32_t >( deviceQueueCreateInfos.size( ) ),
            .pQueueCreateInfos = deviceQueueCreateInfos.data( ),
#ifdef DEBUG
            .enabledLayerCount = static_cast<uint32_t>( layers.size( ) ),
            .ppEnabledLayerNames = layers.data( ),
#elif
            .enabledLayerCount = 0,
#endif
            .enabledExtensionCount = static_cast<uint32_t>(REQUIRED_EXTENSIONS.size( )),
            .ppEnabledExtensionNames = REQUIRED_EXTENSIONS.data( ),
            .pEnabledFeatures = &features
    };

    if ( vkCreateDevice( context->physicalDevice, &createInfo, nullptr, &context->logicalDevice ) != VK_SUCCESS ) {
        TRACE( COMPONENT_VKAPI, VERBOSITY_CRITICAL, "Creation of logical device failed!" )

        throw GraphicsException( GraphicsException::Source::RenderDevice, "Creation of logical device failed!" );
    }

    glfwSetWindowUserPointer( context->window, this );

    context->queues[ QueueType::Graphics ] = { };
    context->queues[ QueueType::Presentation ] = { };

    vkGetDeviceQueue( context->logicalDevice, context->queueFamilies[ QueueType::Graphics ].index, 0,
                      &context->queues[ QueueType::Graphics ] );

    vkGetDeviceQueue( context->logicalDevice, context->queueFamilies[ QueueType::Presentation ].index, 0,
                      &context->queues[ QueueType::Presentation ] );
}

void SomeVulkan::Graphics::RenderDevice::createSurface( ) {
    if ( context->window == nullptr ) {
        throw GraphicsException( GraphicsException::Source::RenderDevice,
                                 "No window context to create renderer surface with." );
    } else if ( glfwCreateWindowSurface( context->instance, context->window, nullptr, &context->surface ) !=
                VK_SUCCESS ) {
        throw GraphicsException( GraphicsException::Source::RenderDevice, "Window surface creation failed." );
    }
}

bool SomeVulkan::Graphics::RenderDevice::defaultDeviceCapabilityCheck( const DeviceInfo &deviceInfo ) {
    int foundRequiredExtensionCount = 0;

    const std::unordered_map< std::string, bool > &reqExtensions = defaultRequiredExtensions( );

    for ( const auto &extension: deviceInfo.extensionProperties ) {

        bool extFound = reqExtensions.find( extension.extensionName ) != reqExtensions.end( );

        foundRequiredExtensionCount += extFound;
    }

    return foundRequiredExtensionCount == reqExtensions.size( );
}

std::unordered_map< std::string, bool > SomeVulkan::Graphics::RenderDevice::defaultRequiredExtensions( ) {
    std::unordered_map< std::string, bool > result;

    result[ VK_KHR_SWAPCHAIN_EXTENSION_NAME ] = true;

    return result;
}

std::vector< VkDeviceQueueCreateInfo > SomeVulkan::Graphics::RenderDevice::createUniqueDeviceCreateInfos( ) {
    std::unordered_map< uint32_t, bool > uniqueIndexes;
    std::vector< VkDeviceQueueCreateInfo > result;

    for ( std::pair< QueueType, QueueFamily > key: context->queueFamilies ) {
        if ( uniqueIndexes.find( key.second.index ) == uniqueIndexes.end( ) ) {
            float priority = key.first == QueueType::Graphics || key.first == QueueType::Presentation ? 1.0f : 0.9f;

            result.emplace_back(
                    VkDeviceQueueCreateInfo {
                            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                            .queueFamilyIndex = key.second.index,
                            .queueCount = 1,
                            .pQueuePriorities = &priority
                    }
            );

            uniqueIndexes[ key.second.index ] = true;
        }
    }

    return result;
}

void SomeVulkan::Graphics::RenderDevice::beforeDelete( ) {
    vkDeviceWaitIdle( context->logicalDevice );
}

std::unique_ptr< SomeVulkan::Graphics::RenderSurface >
SomeVulkan::Graphics::RenderDevice::createRenderSurface( const std::vector< Shader >& shaders ) {
    vkDeviceWaitIdle( context->logicalDevice );

    auto *renderSurface = new RenderSurface { context, shaders };
    return std::unique_ptr< RenderSurface >( renderSurface );
}
SomeVulkan::Graphics::RenderDevice::~RenderDevice( ) {
    if ( debugMessenger != VK_NULL_HANDLE ) {
        auto deleteDebugUtils = ( PFN_vkDestroyDebugUtilsMessengerEXT )
                vkGetInstanceProcAddr( context->instance, "vkDestroyDebugUtilsMessengerEXT" );

        if ( deleteDebugUtils ) {
            deleteDebugUtils( context->instance, debugMessenger, nullptr );
        }
    }

    if ( context->surface != VK_NULL_HANDLE ) {
        vkDestroySurfaceKHR( context->instance, context->surface, nullptr );
    }

    if ( context->logicalDevice != VK_NULL_HANDLE ) {
        vkDestroyDevice( context->logicalDevice, nullptr );
    }

    if ( context->instance != VK_NULL_HANDLE ) {
        vkDestroyInstance( context->instance, nullptr );
    }
}

std::shared_ptr< SomeVulkan::Graphics::RenderContext > SomeVulkan::Graphics::RenderDevice::getContext( ) const {
    return context;
}
