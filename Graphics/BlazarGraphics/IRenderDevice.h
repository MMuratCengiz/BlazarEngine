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
#include "IPipelineProvider.h"
#include "IResourceProvider.h"
#include "IRenderPassProvider.h"
#include "RenderWindow.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct DeviceCapabilities
{
    bool dedicatedTransferQueue;
};

struct DeviceProperties
{
    bool isDedicated;
    uint32_t memoryAvailableInMb;
};

struct DeviceInfo
{
    std::string name;
    DeviceProperties properties;
    DeviceCapabilities capabilities;
};

struct SelectableDevice
{
    DeviceInfo device;
    std::function< void( ) > select;
};

class IRenderDevice
{
public:
    IRenderDevice( ) = default;
    virtual void createDevice( RenderWindow* window ) = 0;
    virtual std::vector< SelectableDevice > listDevices( ) = 0;
    virtual const std::unique_ptr< IPipelineProvider >& getPipelineProvider( ) const = 0;
    virtual const std::unique_ptr< IResourceProvider >& getResourceProvider( ) const = 0;
    virtual const std::unique_ptr< IRenderPassProvider >& getRenderPassProvider( ) const = 0;
    virtual uint32_t getFrameCount( ) const = 0;
    virtual ~IRenderDevice( ) = default;
};

END_NAMESPACES
