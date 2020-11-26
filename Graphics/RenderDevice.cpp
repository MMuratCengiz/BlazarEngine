//
// Created by Murat on 10/9/2020.
//

#include "RenderDevice.h"


NAMESPACES( SomeVulkan, Graphics )

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

RenderDevice::RenderDevice( GLFWwindow *window ) {
    context = std::make_shared< InstanceContext >( );
    context->window = window;

    vk::ApplicationInfo appInfo {
            "SomeVulkan",
            VK_MAKE_VERSION( 1, 0, 0 ),
            "No Engine",
            VK_MAKE_VERSION( 1, 0, 0 ),
            VK_API_VERSION_1_2,
    };

    vk::InstanceCreateInfo createInfo {
            { },
            &appInfo
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

    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo = getDebugUtilsCreateInfo( );

#if DEBUG
    createInfo.pNext = ( vk::DebugUtilsMessengerCreateInfoEXT * ) &debugUtilsCreateInfo;

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

    context->instance = vk::createInstance( createInfo );

    initSupportedExtensions( );
    initDebugMessages( debugUtilsCreateInfo );

    TRACE( COMPONENT_VKAPI, VERBOSITY_INFORMATION, "All vk:: components initialized successfully." )

    createSurface( );
}

void RenderDevice::initSupportedExtensions( ) {
    auto extensionProperties = vk::enumerateInstanceExtensionProperties( nullptr );

    for ( vk::ExtensionProperties prp: extensionProperties ) {
        this->supportedExtensions[ prp.extensionName ] = true;
    }
}

void RenderDevice::initSupportedLayers( std::vector< const char * > &layers ) {
    auto layerProperties = vk::enumerateInstanceLayerProperties( );

    for ( vk::LayerProperties prp: layerProperties ) {
        auto layerPair = ENABLED_LAYERS.find( prp.layerName );

        if ( layerPair != ENABLED_LAYERS.end( ) ) {
            supportedLayers[ prp.layerName ] = true;
            layers.emplace_back( layerPair->first.c_str( ) );
        }
    }
}

vk::DebugUtilsMessengerCreateInfoEXT RenderDevice::getDebugUtilsCreateInfo( ) const {
    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo { };

    debugUtilsCreateInfo.setMessageSeverity(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning );

    debugUtilsCreateInfo.setMessageType(
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance );

    debugUtilsCreateInfo.setPfnUserCallback( debugCallback );
    return debugUtilsCreateInfo;
}

void RenderDevice::initDebugMessages( const vk::DebugUtilsMessengerCreateInfoEXT &createInfo ) {
    auto instance = static_cast< VkInstance >( context->instance );

    auto createDebugUtils = ( PFN_vkCreateDebugUtilsMessengerEXT ) vkGetInstanceProcAddr(
            instance, "vkCreateDebugUtilsMessengerEXT" );

    auto createInfoCast = static_cast< VkDebugUtilsMessengerCreateInfoEXT >( createInfo );

    if ( createDebugUtils == nullptr ||
         createDebugUtils( instance, &createInfoCast, nullptr, &debugMessenger ) != VK_SUCCESS ) {
        TRACE( COMPONENT_VKAPI, VERBOSITY_HIGH, "Failed to create Vk debugger." )
        throw GraphicsException( GraphicsException::Source::RenderDevice, "Failed to initialize debugger!" );
    }
}

std::vector< DeviceInfo >
RenderDevice::listGPUs( T_FUNC::deviceCapabilityCheck deviceCapabilityCheck ) {
    auto devices = context->instance.enumeratePhysicalDevices( );
    std::vector< DeviceInfo > result;

    for ( auto it = devices.begin( ); it != devices.end( ); ) {
        vk::PhysicalDevice physicalDevice = *it;

        DeviceInfo deviceInfo{ physicalDevice };

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
RenderDevice::createDeviceInfo( const vk::PhysicalDevice &physicalDevice, DeviceInfo &deviceInfo ) {
    vk::PhysicalDeviceFeatures deviceFeatures;
    vk::PhysicalDeviceProperties deviceProperties;

    std::vector< vk::QueueFamilyProperties > localQueueFamilies;

    physicalDevice.getProperties( &deviceProperties );
    physicalDevice.getFeatures( &deviceFeatures );

    uint32_t queueFamilyCount;
    physicalDevice.getQueueFamilyProperties( &queueFamilyCount, nullptr );
    physicalDevice.getQueueFamilyProperties( &queueFamilyCount, localQueueFamilies.data( ) );

    auto extensions = physicalDevice.enumerateDeviceExtensionProperties( nullptr );

    deviceInfo.properties = deviceProperties;
    deviceInfo.features = deviceFeatures;
    deviceInfo.extensionProperties = extensions;
}

void RenderDevice::selectDevice( const DeviceInfo &deviceInfo ) {
    context->physicalDevice = deviceInfo.device;
    createLogicalDevice( );
    initializeVMA( );
    createRenderPass( );
}

void RenderDevice::setupQueueFamilies( ) {
    auto exists = [ & ](  QueueType bit ) -> bool {
        return context->queueFamilies.find( bit ) != context->queueFamilies.end( );
    };

    auto localQueueFamilies = context->physicalDevice.getQueueFamilyProperties( );

    uint32_t index = 0;
    for ( const vk::QueueFamilyProperties& property: localQueueFamilies ) {
        bool hasGraphics = ( property.queueFlags & vk::QueueFlagBits::eGraphics ) == vk::QueueFlagBits::eGraphics;
        bool hasTransfer = ( property.queueFlags & vk::QueueFlagBits::eTransfer ) == vk::QueueFlagBits::eTransfer;

        if ( hasGraphics && !exists( QueueType::Graphics ) ) {
            context->queueFamilies[ QueueType::Graphics ] = QueueFamily { index, property };
        } else if ( hasTransfer && !exists( QueueType::Transfer ) ) { // Try to fetch a unique transfer queue
            context->queueFamilies[ QueueType::Transfer ] = QueueFamily { index, property };
        }

        vk::Bool32 presentationSupport = context->physicalDevice.getSurfaceSupportKHR( index, context->surface );

        if ( presentationSupport && !exists( QueueType::Presentation ) ) {
            context->queueFamilies[ QueueType::Presentation ] = QueueFamily { index, property };
        }

        ++index;
    }

    if ( !exists( QueueType::Transfer ) ) {
        context->queueFamilies[ QueueType::Transfer ] = context->queueFamilies[ QueueType::Graphics ];
    }
}

void RenderDevice::createLogicalDevice( ) {
    setupQueueFamilies( );

    std::vector< vk::DeviceQueueCreateInfo > deviceQueueCreateInfos = createUniqueDeviceCreateInfos( );

    vk::PhysicalDeviceFeatures features { };
    features.samplerAnisotropy = true;
    features.sampleRateShading = true;

#ifdef DEBUG
    std::vector< const char * > layers;
    initSupportedLayers( layers );
#endif
    vk::DeviceCreateInfo createInfo {
            vk::DeviceCreateFlagBits( ),
            static_cast< uint32_t >( deviceQueueCreateInfos.size( ) ),
            deviceQueueCreateInfos.data( ),
#ifdef DEBUG
            static_cast<uint32_t>( layers.size( ) ),
            layers.data( ),
#elif
            0, { },
#endif
            static_cast<uint32_t>(REQUIRED_EXTENSIONS.size( )),
            REQUIRED_EXTENSIONS.data( ),
            &features
    };

    context->logicalDevice = context->physicalDevice.createDevice( createInfo );

    glfwSetWindowUserPointer( context->window, this );

    context->queues[ QueueType::Graphics ] = vk::Queue{ };
    context->queues[ QueueType::Presentation ] = vk::Queue{ };
    context->queues[ QueueType::Transfer ] = vk::Queue{ };

    context->logicalDevice.getQueue( context->queueFamilies[ QueueType::Graphics ].index, 0,
                                     &context->queues[ QueueType::Graphics ] );

    context->logicalDevice.getQueue( context->queueFamilies[ QueueType::Presentation ].index, 0,
                                     &context->queues[ QueueType::Presentation ] );

    context->logicalDevice.getQueue( context->queueFamilies[ QueueType::Transfer ].index, 0,
                                     &context->queues[ QueueType::Transfer ] );
}

void RenderDevice::initializeVMA( ) {
    vma::AllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_1;
    allocatorInfo.physicalDevice = context->physicalDevice;
    allocatorInfo.device = context->logicalDevice;
    allocatorInfo.instance = context->instance ;

    context->vma = vma::createAllocator( allocatorInfo );
}

void RenderDevice::createSurface( ) {
    auto instance = static_cast< VkInstance >( context->instance );
    auto surface = static_cast< VkSurfaceKHR >( context->surface ); // TODO Check this might be problematic

    if ( context->window == nullptr ) {
        throw GraphicsException( GraphicsException::Source::RenderDevice,
                                 "No window context to create renderer surface with." );
    } else if ( glfwCreateWindowSurface( instance, context->window, nullptr, &surface ) != VK_SUCCESS ) {
        throw GraphicsException( GraphicsException::Source::RenderDevice, "Window surface creation failed." );
    }

    context->surface = vk::SurfaceKHR( surface );
}

bool RenderDevice::defaultDeviceCapabilityCheck( const DeviceInfo &deviceInfo ) {
    int foundRequiredExtensionCount = 0;

    const std::unordered_map< std::string, bool > &reqExtensions = defaultRequiredExtensions( );

    for ( const auto &extension: deviceInfo.extensionProperties ) {
        bool extFound = reqExtensions.find( extension.extensionName ) != reqExtensions.end( );
        foundRequiredExtensionCount += extFound;
    }

    return foundRequiredExtensionCount == reqExtensions.size( );
}

std::unordered_map< std::string, bool > RenderDevice::defaultRequiredExtensions( ) {
    std::unordered_map< std::string, bool > result;
    result[ VK_KHR_SWAPCHAIN_EXTENSION_NAME ] = true;
    return result;
}

std::vector< vk::DeviceQueueCreateInfo > RenderDevice::createUniqueDeviceCreateInfos( ) {
    std::unordered_map< uint32_t, bool > uniqueIndexes;
    std::vector< vk::DeviceQueueCreateInfo > result;

    for ( std::pair< QueueType, QueueFamily > key: context->queueFamilies ) {
        if ( uniqueIndexes.find( key.second.index ) == uniqueIndexes.end( ) ) {
            float priority = key.first == QueueType::Graphics || key.first == QueueType::Presentation ? 1.0f : 0.9f;

            result.emplace_back(
                    vk::DeviceQueueCreateInfo {
                            vk::DeviceQueueCreateFlagBits( ),
                            key.second.index,
                            1,
                            &priority
                    }
            );

            uniqueIndexes[ key.second.index ] = true;
        }
    }

    return result;
}

void RenderDevice::beforeDelete( ) {
    context->logicalDevice.waitIdle( );
}

std::unique_ptr< RenderSurface >
RenderDevice::createRenderSurface( const std::shared_ptr< Scene::Camera >& camera ) {
    context->logicalDevice.waitIdle( );

    auto *renderSurface = new RenderSurface { context, camera };
    return std::unique_ptr< RenderSurface >( renderSurface );
}

RenderDevice::~RenderDevice( ) {
    destroyDebugUtils( );

    context->instance.destroySurfaceKHR( context->surface );
    context->vma.destroy();
    context->logicalDevice.destroy( );
    context->instance.destroy( );
}

void RenderDevice::destroyDebugUtils( ) const {
    if ( debugMessenger == VK_NULL_HANDLE ) {
        return;
    }

    auto instance = static_cast<VkInstance>(context->instance);
    auto deleteDebugUtils = ( PFN_vkDestroyDebugUtilsMessengerEXT )
            vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" );

    if ( deleteDebugUtils ) {
        deleteDebugUtils( static_cast< VkInstance >( instance ), debugMessenger, nullptr );
    }
}

std::shared_ptr< InstanceContext > RenderDevice::getContext( ) const {
    return context;
}

void RenderDevice::createRenderPass( ) {
    auto surfaceFormats = context->physicalDevice.getSurfaceFormatsKHR( context->surface );
    auto presentModes = context->physicalDevice.getSurfacePresentModesKHR( context->surface );

    auto presentMode = vk::PresentModeKHR::eFifo;
    for ( auto mode: presentModes ) {
        if ( mode == vk::PresentModeKHR::eMailbox ) {
            presentMode = mode;
        }
    }

    auto surfaceFormat = vk::SurfaceFormatKHR{ vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear };
    for ( auto format: surfaceFormats ) {
        if ( format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear ) {
            surfaceFormat = format;
        }
    }

    context->imageFormat = surfaceFormat.format;
    context->colorSpace = surfaceFormat.colorSpace;
    context->presentMode = presentMode;

    // Color Attachment
    auto msaaSampleCount = RenderUtilities::maxDeviceMSAASampleCount( context->physicalDevice );
    vk::AttachmentDescription colorAttachmentDescription { };
    colorAttachmentDescription.format = context->imageFormat;
    colorAttachmentDescription.samples = msaaSampleCount;
    colorAttachmentDescription.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachmentDescription.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachmentDescription.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachmentDescription.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentReference colorAttachmentReference { };
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = vk::ImageLayout::eColorAttachmentOptimal;
    // --

    // Depth attachment
    vk::AttachmentDescription depthAttachmentDescription { };
    depthAttachmentDescription.format = RenderUtilities::findSupportedDepthFormat( context->physicalDevice );
    depthAttachmentDescription.samples = msaaSampleCount;
    depthAttachmentDescription.loadOp = vk::AttachmentLoadOp::eClear;
    depthAttachmentDescription.storeOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachmentDescription.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depthAttachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachmentDescription.initialLayout = vk::ImageLayout::eUndefined;
    depthAttachmentDescription.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::AttachmentReference depthAttachmentReference { };
    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    // --

    // Color Image Resolver for MSAA
    vk::AttachmentDescription colorAttachmentResolve { };
    colorAttachmentResolve.format = context->imageFormat;
    colorAttachmentResolve.samples = vk::SampleCountFlagBits::e1;
    colorAttachmentResolve.loadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentResolve.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachmentResolve.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachmentResolve.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachmentResolve.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachmentResolve.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colorAttachmentResolveReference { };
    colorAttachmentResolveReference.attachment = 2;
    colorAttachmentResolveReference.layout = vk::ImageLayout::eColorAttachmentOptimal;
    // --

    vk::SubpassDescription subPass { };
    subPass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subPass.colorAttachmentCount = 1;
    subPass.pColorAttachments = &colorAttachmentReference;
    subPass.pDepthStencilAttachment = &depthAttachmentReference;
    subPass.pResolveAttachments = &colorAttachmentResolveReference;

    std::array< vk::AttachmentDescription, 3 > attachments {
        colorAttachmentDescription, depthAttachmentDescription, colorAttachmentResolve };

    vk::SubpassDependency dependency { };
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = { }; // TODO Recheck
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    vk::RenderPassCreateInfo renderPassCreateInfo { };
    renderPassCreateInfo.attachmentCount = attachments.size( );
    renderPassCreateInfo.pAttachments = attachments.data( );
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subPass;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;

    context->renderPass = context->logicalDevice.createRenderPass( renderPassCreateInfo );
}

END_NAMESPACES