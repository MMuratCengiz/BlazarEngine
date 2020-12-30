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
