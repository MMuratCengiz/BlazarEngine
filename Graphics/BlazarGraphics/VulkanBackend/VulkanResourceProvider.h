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

#include <BlazarCore/Common.h>
#include "VulkanCommandExecutor.h"
#include "VulkanUtilities.h"
#include "VulkanContext.h"
#include "VulkanSamplerAllocator.h"
#include "VulkanCubeMapAllocator.h"
#include "../IResourceProvider.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct VulkanBufferWrapper
{
    std::pair< vk::Buffer, vma::Allocation > buffer;
};

class VulkanResourceLock : public IResourceLock
{
private:
    vk::Fence fence { };
    vk::Semaphore semaphore { };

    VulkanContext* context;
    bool resourceCleaned = false;
public:
    VulkanResourceLock( VulkanContext* context, const ResourceLockType &lockType );
    void wait( ) override;
    void reset( ) override;
    void notify( ) override;

    const vk::Fence &getVkFence( );
    const vk::Semaphore &getVkSemaphore( );

    void cleanup( ) override;
    ~VulkanResourceLock( ) override;
};

class VulkanResourceProvider : public IResourceProvider
{
    VulkanContext * context;
    std::shared_ptr< VulkanCommandExecutor > commandExecutor;
public:
    inline explicit VulkanResourceProvider( VulkanContext * context ) : context( context )
    {
        commandExecutor = std::make_shared< VulkanCommandExecutor >( this->context );
    }

    std::shared_ptr< ShaderResource > createResource( const ShaderResourceRequest &request ) override;
    std::shared_ptr< IResourceLock > createLock( const ResourceLockType &lockType ) override;

    void createBufferAllocator( const std::shared_ptr< ShaderResource > &resource );
    void createSampler2DAllocator( const std::shared_ptr< ShaderResource > &resource );
    void createCubeMapAllocator( const std::shared_ptr< ShaderResource > &resource );

    ~VulkanResourceProvider( ) override = default;
};

END_NAMESPACES