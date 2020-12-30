#pragma once

#include "VulkanContext.h"
#include "VulkanUtilities.h"
#include "GLSLShaderSet.h"
#include "BlazarECS/CCamera.h"
#include <BlazarInput/GlobalEventHandler.h>
#include <BlazarECS/ECS.h>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class VulkanSurface
{
private:
    VulkanContext * context;
public:
    explicit VulkanSurface( VulkanContext * context );

    ~VulkanSurface( );
private:
    void createSurface( );
    void updateViewport( const uint32_t &width, const uint32_t &height );
    void createSwapChain( const vk::SurfaceCapabilitiesKHR &surfaceCapabilities );
    void createImageView( vk::ImageView &imageView, const vk::Image &image, const vk::Format &format, const vk::ImageAspectFlags &aspectFlags );
    void chooseExtent2D( const vk::SurfaceCapabilitiesKHR &capabilities );
    void createSwapChainImages( vk::Format format );
    void dispose( );
};
END_NAMESPACES