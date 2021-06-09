#include "VulkanDevice.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                     VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                     const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                     void *pUserData )
{
    int verbosity;

    switch ( messageSeverity )
    {
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

    TRACE( "RenderDeviceVulkan", verbosity, pCallbackData->pMessage )

    return VK_FALSE;
}

void VulkanDevice::loadExtensionFunctions( )
{
    vk::DynamicLoader dl;
    auto vkGetInstanceProcAddr = dl.getProcAddress< PFN_vkGetInstanceProcAddr >( "vkGetInstanceProcAddr" );
    VULKAN_HPP_DEFAULT_DISPATCHER.init( vkGetInstanceProcAddr );
}

void VulkanDevice::createDevice( RenderWindow *window )
{
    loadExtensionFunctions( );

    context = std:: make_unique< VulkanContext >( );
    context->window = window;

    vk::ApplicationInfo appInfo
            {
                    "BlazarEngine",
                    VK_MAKE_VERSION( 1, 0, 0 ),
                    "No Engine",
                    VK_MAKE_VERSION( 1, 0, 0 ),
                    VK_API_VERSION_1_2,
            };

    vk::InstanceCreateInfo createInfo
            {
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

    if ( supportedLayers.find( "VK_LAYER_KHRONOS_validation" ) == supportedLayers.end( ) )
    {
        createInfo.enabledLayerCount = 0;
        TRACE( COMPONENT_VKAPI, VERBOSITY_INFORMATION, "Layer: VK_LAYER_KHRONOS_validation not found." )
    }
    else
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>( layers.size( ));
        createInfo.ppEnabledLayerNames = layers.data( );
    }
#elif
    createInfo.enabledLayerCount = 0;
#endif

    context->instance = vk::createInstance( createInfo );
    VULKAN_HPP_DEFAULT_DISPATCHER.init( context->instance );

    initSupportedExtensions( );
    initDebugMessages( debugUtilsCreateInfo );

    TRACE( COMPONENT_VKAPI, VERBOSITY_INFORMATION, "All vk:: components initialized successfully." )

    createSurface( );
}

void VulkanDevice::initSupportedExtensions( )
{
    auto extensionProperties = vk::enumerateInstanceExtensionProperties( nullptr );

    for ( vk::ExtensionProperties prp: extensionProperties )
    {
        this->supportedExtensions[ prp.extensionName ] = true;
    }
}

void VulkanDevice::initSupportedLayers( std::vector< const char * > &layers )
{
    auto layerProperties = vk::enumerateInstanceLayerProperties( );

    for ( vk::LayerProperties prp: layerProperties )
    {
        auto layerPair = ENABLED_LAYERS.find( prp.layerName );

        if ( layerPair != ENABLED_LAYERS.end( ) )
        {
            supportedLayers[ prp.layerName ] = true;
            layers.emplace_back( layerPair->first.c_str( ) );
        }
    }
}

vk::DebugUtilsMessengerCreateInfoEXT VulkanDevice::getDebugUtilsCreateInfo( ) const
{
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

void VulkanDevice::initDebugMessages( const vk::DebugUtilsMessengerCreateInfoEXT &createInfo )
{
    auto instance = static_cast< VkInstance >( context->instance );

    auto createDebugUtils = ( PFN_vkCreateDebugUtilsMessengerEXT ) vkGetInstanceProcAddr(
            instance, "vkCreateDebugUtilsMessengerEXT" );

    auto createInfoCast = static_cast< VkDebugUtilsMessengerCreateInfoEXT >( createInfo );

    if ( createDebugUtils == nullptr ||
         createDebugUtils( instance, &createInfoCast, nullptr, &debugMessenger ) != VK_SUCCESS )
    {
        TRACE( COMPONENT_VKAPI, VERBOSITY_HIGH, "Failed to create Vk debugger." )
        throw GraphicsException( "RenderDevice", "Failed to initialize debugger!" );
    }
}

std::vector< SelectableDevice > VulkanDevice::listDevices( )
{
    auto devices = context->instance.enumeratePhysicalDevices( );
    std::vector< SelectableDevice > result;

    for (auto & device : devices)
    {
        vk::PhysicalDevice physicalDevice = device;

        SelectableDevice selectableDevice { };

        createDeviceInfo( physicalDevice, selectableDevice.device );

        selectableDevice.select = [ = ]( )
        {
            selectDevice( device );
        };

        result.push_back( selectableDevice );
    }

    return result;
}

void VulkanDevice::createDeviceInfo( const vk::PhysicalDevice &physicalDevice, DeviceInfo &deviceInfo )
{
    vk::PhysicalDeviceFeatures deviceFeatures;
    vk::PhysicalDeviceProperties deviceProperties;

    std::vector< vk::QueueFamilyProperties > localQueueFamilies;

    physicalDevice.getProperties( &deviceProperties );
    physicalDevice.getFeatures( &deviceFeatures );

    uint32_t queueFamilyCount;
    physicalDevice.getQueueFamilyProperties( &queueFamilyCount, nullptr );
    physicalDevice.getQueueFamilyProperties( &queueFamilyCount, localQueueFamilies.data( ) );

    auto extensions = physicalDevice.enumerateDeviceExtensionProperties( nullptr );

    deviceInfo.name = std::string( deviceProperties.deviceName.data( ) );
    deviceInfo.properties.isDedicated = true; // todo
    deviceInfo.capabilities.dedicatedTransferQueue = true; // todo
}

void VulkanDevice::selectDevice( const vk::PhysicalDevice &device )
{
    // Todo break if already initialized
    context->physicalDevice = device;

    createLogicalDevice( );
    initializeVMA( );
    createImageFormat( );
    createRenderSurface( );

    vk::CommandPoolCreateInfo graphicsCommandPoolCreateInfo { };
    graphicsCommandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    graphicsCommandPoolCreateInfo.queueFamilyIndex = context->queueFamilies[ QueueType::Graphics ].index;

    vk::CommandPoolCreateInfo transferCommandPoolCreateInfo { };
    transferCommandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    transferCommandPoolCreateInfo.queueFamilyIndex = context->queueFamilies[ QueueType::Transfer ].index;

    context->graphicsQueueCommandPool = context->logicalDevice.createCommandPool( graphicsCommandPoolCreateInfo );
    context->transferQueueCommandPool = context->logicalDevice.createCommandPool( transferCommandPoolCreateInfo );

    pipelineProvider = std::make_unique< VulkanPipelineProvider >( context.get( ) );
    renderPassProvider = std::make_unique< VulkanRenderPassProvider >( context.get( ) );
    resourceProvider = std::make_unique< VulkanResourceProvider >( context.get( ) );
}

void VulkanDevice::setupQueueFamilies( )
{
    auto exists = [ & ]( QueueType bit ) -> bool
    {
        return context->queueFamilies.find( bit ) != context->queueFamilies.end( );
    };

    auto localQueueFamilies = context->physicalDevice.getQueueFamilyProperties( );

    uint32_t index = 0;
    for ( const vk::QueueFamilyProperties &property: localQueueFamilies )
    {
        bool hasGraphics = ( property.queueFlags & vk::QueueFlagBits::eGraphics ) == vk::QueueFlagBits::eGraphics;
        bool hasTransfer = ( property.queueFlags & vk::QueueFlagBits::eTransfer ) == vk::QueueFlagBits::eTransfer;

        if ( hasGraphics && !exists( QueueType::Graphics ) )
        {
            context->queueFamilies[ QueueType::Graphics ] = QueueFamily { index, property };
        }
        else if ( hasTransfer && !exists( QueueType::Transfer ) )
        { // Try to fetch a unique transfer queue
            context->queueFamilies[ QueueType::Transfer ] = QueueFamily { index, property };
        }

        vk::Bool32 presentationSupport = context->physicalDevice.getSurfaceSupportKHR( index, context->surface );

        if ( presentationSupport && !exists( QueueType::Presentation ) )
        {
            context->queueFamilies[ QueueType::Presentation ] = QueueFamily { index, property };
        }

        ++index;
    }

    if ( !exists( QueueType::Transfer ) )
    {
        context->queueFamilies[ QueueType::Transfer ] = context->queueFamilies[ QueueType::Graphics ];
    }
}

void VulkanDevice::createLogicalDevice( )
{
    setupQueueFamilies( );

    std::vector< vk::DeviceQueueCreateInfo > deviceQueueCreateInfos = createUniqueDeviceCreateInfos( );

    vk::PhysicalDeviceFeatures features { };
    features.samplerAnisotropy = true;
    features.sampleRateShading = true;
    features.tessellationShader = true;

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
    VULKAN_HPP_DEFAULT_DISPATCHER.init( context->logicalDevice );

    context->queues[ QueueType::Graphics ] = vk::Queue { };
    context->queues[ QueueType::Presentation ] = vk::Queue { };
    context->queues[ QueueType::Transfer ] = vk::Queue { };

    context->logicalDevice.getQueue( context->queueFamilies[ QueueType::Graphics ].index, 0,
                                     &context->queues[ QueueType::Graphics ] );

    context->logicalDevice.getQueue( context->queueFamilies[ QueueType::Presentation ].index, 0,
                                     &context->queues[ QueueType::Presentation ] );

    context->logicalDevice.getQueue( context->queueFamilies[ QueueType::Transfer ].index, 0,
                                     &context->queues[ QueueType::Transfer ] );
}

void VulkanDevice::initializeVMA( )
{
    vma::AllocatorCreateInfo allocatorInfo = { };
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_1;
    allocatorInfo.physicalDevice = context->physicalDevice;
    allocatorInfo.device = context->logicalDevice;
    allocatorInfo.instance = context->instance;

    context->vma = vma::createAllocator( allocatorInfo );
}

void VulkanDevice::createSurface( )
{
    auto instance = static_cast< VkInstance >( context->instance );
    auto surface = static_cast< VkSurfaceKHR >( context->surface );

#if WIN32
    auto createWindowSurface = ( PFN_vkCreateWin32SurfaceKHR ) vkGetInstanceProcAddr( instance, "vkCreateWin32SurfaceKHR" );

    if ( !createWindowSurface )
    {
        throw GraphicsException( "RenderDevice", "Window surface creation failed." );
    }

    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo { };
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = context->window->getHInstance( );
    surfaceCreateInfo.hwnd = context->window->getHWindow( );
    createWindowSurface( instance, &surfaceCreateInfo, nullptr, &surface );
#endif

    context->surface = vk::SurfaceKHR( surface );
}

std::unordered_map< std::string, bool > VulkanDevice::defaultRequiredExtensions( )
{
    std::unordered_map< std::string, bool > result;
    result[ VK_KHR_SWAPCHAIN_EXTENSION_NAME ] = true;
    return result;
}

std::vector< vk::DeviceQueueCreateInfo > VulkanDevice::createUniqueDeviceCreateInfos( )
{
    std::unordered_map< uint32_t, bool > uniqueIndexes;
    std::vector< vk::DeviceQueueCreateInfo > result;

    for ( std::pair< QueueType, QueueFamily > key: context->queueFamilies )
    {
        if ( uniqueIndexes.find( key.second.index ) == uniqueIndexes.end( ) )
        {
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

void VulkanDevice::beforeDelete( )
{
    context->logicalDevice.waitIdle( );
}

void VulkanDevice::createRenderSurface( )
{
    context->logicalDevice.waitIdle( );

    auto *renderSurfacePtr = new VulkanSurface { context.get( ) };
    this->renderSurface = std::unique_ptr< VulkanSurface >( renderSurfacePtr );
}

VulkanDevice::~VulkanDevice( )
{
    resourceProvider.reset( );
    pipelineProvider.reset( );
    renderPassProvider.reset( );

    renderSurface.reset( );

    destroyDebugUtils( );
    context->logicalDevice.destroyCommandPool( context->transferQueueCommandPool );
    context->logicalDevice.destroyCommandPool( context->graphicsQueueCommandPool );
    context->logicalDevice.destroyCommandPool( context->computeQueueCommandPool );

    context->instance.destroySurfaceKHR( context->surface );
    context->vma.destroy( );
    context->logicalDevice.destroy( );
    context->instance.destroy( );
}

void VulkanDevice::destroyDebugUtils( ) const
{
    if ( debugMessenger == VK_NULL_HANDLE )
    {
        return;
    }

    auto instance = static_cast<VkInstance>(context->instance);
    auto deleteDebugUtils = ( PFN_vkDestroyDebugUtilsMessengerEXT ) vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" );

    if ( deleteDebugUtils )
    {
        deleteDebugUtils( static_cast< VkInstance >( instance ), debugMessenger, nullptr );
    }
}

const VulkanContext * VulkanDevice::getContext( ) const
{
    return context.get( );
}

void VulkanDevice::createImageFormat( )
{
    auto surfaceFormats = context->physicalDevice.getSurfaceFormatsKHR( context->surface );
    auto presentModes = context->physicalDevice.getSurfacePresentModesKHR( context->surface );

    auto presentMode = vk::PresentModeKHR::eFifo;
    for ( auto mode: presentModes )
    {
        if ( mode == vk::PresentModeKHR::eMailbox )
        {
            presentMode = mode;
        }
    }

    auto surfaceFormat = vk::SurfaceFormatKHR { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
    for ( auto format: surfaceFormats )
    {
        if ( format.format == vk::Format::eB8G8R8A8Unorm /*&& format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear*/ )
        {
            surfaceFormat = format;
        }
    }

    context->imageFormat = surfaceFormat.format;
    context->colorSpace = surfaceFormat.colorSpace;
    context->presentMode = presentMode;
}

const std::unique_ptr< IPipelineProvider > &VulkanDevice::getPipelineProvider( ) const
{
    return pipelineProvider;
}

const std::unique_ptr< IRenderPassProvider > &VulkanDevice::getRenderPassProvider( ) const
{
    return renderPassProvider;
}

const std::unique_ptr< IResourceProvider > &VulkanDevice::getResourceProvider( ) const
{
    return resourceProvider;
}

uint32_t VulkanDevice::getFrameCount( ) const
{
    return context->swapChainImages.size( );
}

END_NAMESPACES