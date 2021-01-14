#pragma once

#include <BlazarCore/Common.h>
#include "Pass.h"
#include "../BuiltinPrimitives.h"
#include "../IRenderDevice.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class CommonPasses
{
private:
    CommonPasses( ) = default;
public:
    static std::shared_ptr< Pass > createGBufferPass(  IRenderDevice* renderDevice );
    static std::shared_ptr< Pass > createShadowMapPass(  IRenderDevice* renderDevice );
    static std::shared_ptr< Pass > createLightingPass( IRenderDevice* renderDevice );
    static std::shared_ptr< Pass > createSkyBoxPass( IRenderDevice* renderDevice );
    static std::shared_ptr< Pass > createPresentPass( IRenderDevice* renderDevice );
};

END_NAMESPACES

