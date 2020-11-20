//
// Created by Murat on 10/22/2020.
//

#include "RenderDeviceBuilder.h"

NAMESPACES( SomeVulkan, Graphics )

RenderDevice *RenderDeviceBuilder::createRenderDevice( GLFWwindow *window ) {
    return new RenderDevice { window };
}
RenderDeviceBuilder::RenderDeviceSelectDevice::RenderDeviceSelectDevice( std::shared_ptr< RenderDevice > renderDevice )
        : renderDevice(
        std::move( renderDevice ) ) { }

RenderDeviceBuilder::RenderDeviceComplete::RenderDeviceComplete( std::shared_ptr< RenderDevice > renderDevice )
        : renderDevice(
        std::move( renderDevice ) ) { }


RenderDeviceBuilder::RenderDeviceSelectDevice
RenderDeviceBuilder::RenderDeviceSetWindow::selectWindow( GLFWwindow *window ) {
    auto *rd = createRenderDevice(  window );
    this->renderDevice = std::shared_ptr< RenderDevice >( rd );
    return RenderDeviceSelectDevice { this->renderDevice };
}

RenderDeviceBuilder::RenderDeviceComplete RenderDeviceBuilder::RenderDeviceSelectDevice::selectDevice(
        const std::function< DeviceInfo( std::vector< DeviceInfo > ) > &deviceSelector ) {
    DeviceInfo selectedDevice = deviceSelector( renderDevice->listGPUs( ) );
    renderDevice->selectDevice( selectedDevice );
    return RenderDeviceComplete { renderDevice };
}

std::shared_ptr< RenderDevice > RenderDeviceBuilder::RenderDeviceComplete::create( ) const {
    return renderDevice;
}

END_NAMESPACES