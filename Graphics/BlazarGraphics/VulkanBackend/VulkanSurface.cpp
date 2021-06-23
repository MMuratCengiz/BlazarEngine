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

#include <fstream>
#include <utility>
#include "VulkanSurface.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

VulkanSurface::VulkanSurface( VulkanContext * context ) : context( context )
{
    createSurface( );

    Input::GlobalEventHandler::Instance( ).subscribeToEvent( Input::EventType::SwapChainInvalidated, [ & ]( const Input::EventType &eventType, std::shared_ptr< Input::IEventParameters > eventParams )
    {
        this->context->logicalDevice.waitIdle( );

        dispose( );
        createSurface( );
    } );
}

void VulkanSurface::createSurface( )
{
    vk::SurfaceCapabilitiesKHR capabilities;

    capabilities = context->physicalDevice.getSurfaceCapabilitiesKHR( context->surface );

    createSwapChain( capabilities );
}

void VulkanSurface::createSwapChain( const vk::SurfaceCapabilitiesKHR &surfaceCapabilities )
{
    chooseExtent2D( surfaceCapabilities );

    vk::SwapchainCreateInfoKHR createInfo { };

    uint32_t imageCount = std::min( surfaceCapabilities.maxImageCount, surfaceCapabilities.minImageCount + 1 );

    createInfo.surface = context->surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = context->imageFormat;
    createInfo.imageColorSpace = context->colorSpace;
    createInfo.imageExtent = context->surfaceExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    const uint32_t qfIndexes[2] = { context->queueFamilies[ QueueType::Graphics ].index, context->queueFamilies[ QueueType::Presentation ].index };

    if ( qfIndexes[ 0 ] != qfIndexes[ 1 ] )
    {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = qfIndexes;
    }
    else
    {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = surfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = context->presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = context->swapChain;

    context->swapChain = context->logicalDevice.createSwapchainKHR( createInfo );

    createSwapChainImages( context->imageFormat );
}

void VulkanSurface::createSwapChainImages( vk::Format format )
{
    context->swapChainImages = context->logicalDevice.getSwapchainImagesKHR( context->swapChain );

    context->swapChainImageViews.resize( context->swapChainImages.size( ) );

    int index = 0;
    for ( auto image: context->swapChainImages )
    {
        createImageView( context->swapChainImageViews[ index++ ], image, format, vk::ImageAspectFlagBits::eColor );
    }
}

void VulkanSurface::chooseExtent2D( const vk::SurfaceCapabilitiesKHR &capabilities )
{
    if ( capabilities.currentExtent.width != UINT32_MAX )
    {
        context->surfaceExtent.width = capabilities.currentExtent.width;
        context->surfaceExtent.height = capabilities.currentExtent.height;
        return;
    }

    context->surfaceExtent.width = std::clamp( context->window->getWidth( ), capabilities.minImageExtent.width, capabilities.maxImageExtent.width );
    context->surfaceExtent.height = std::clamp( context->window->getHeight( ), capabilities.minImageExtent.height, capabilities.maxImageExtent.height );
}

void VulkanSurface::createImageView( vk::ImageView &imageView, const vk::Image &image,
                                     const vk::Format &format, const vk::ImageAspectFlags &aspectFlags
)
{
    vk::ImageViewCreateInfo imageViewCreateInfo { };
    imageViewCreateInfo.image = image;
    imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
    imageViewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
    imageViewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
    imageViewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;
    imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    imageView = context->logicalDevice.createImageView( imageViewCreateInfo );
}

VulkanSurface::~VulkanSurface( )
{
    dispose( );
    context->logicalDevice.destroySwapchainKHR( context->swapChain );
}

void VulkanSurface::dispose( )
{
    for ( auto &imageView: context->swapChainImageViews )
    {
        context->logicalDevice.destroyImageView( imageView );
    }
}

END_NAMESPACES