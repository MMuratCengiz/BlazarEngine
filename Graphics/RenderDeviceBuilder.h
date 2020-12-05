#pragma once

#include "RenderDevice.h"
#include <memory>
#include <utility>

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct DeviceInfo;
class RenderDevice;

class RenderDeviceBuilder {
private:
    class RenderDeviceSelectDevice;

    class RenderDeviceSetWindow;

    class RenderDeviceComplete;

public:
    friend class RenderDevice;

    static RenderDeviceSetWindow create( ) {
        return RenderDeviceSetWindow { };
    }

private:
    static RenderDevice * createRenderDevice( GLFWwindow *window );

    class RenderDeviceSetWindow {
    public:
        std::shared_ptr< RenderDevice > renderDevice;

        RenderDeviceSelectDevice selectWindow( GLFWwindow *window );
    };

    class RenderDeviceSelectDevice {
    public:
        std::shared_ptr< RenderDevice > renderDevice;

        explicit RenderDeviceSelectDevice( std::shared_ptr< RenderDevice > renderDevice );

        RenderDeviceComplete
        selectDevice( const std::function< DeviceInfo( std::vector< DeviceInfo > ) > &deviceSelector );
    };

    class RenderDeviceComplete {
    public:
        std::shared_ptr< RenderDevice > renderDevice;

        explicit RenderDeviceComplete( std::shared_ptr< RenderDevice > renderDevice );

        [[nodiscard]] std::shared_ptr< RenderDevice > create( ) const;
    };
};

END_NAMESPACES