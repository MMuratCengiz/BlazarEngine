#pragma once

#include <BlazarGraphics/RenderGraph/Pass.h>

class TDGamePasses
{
public:
    static std::unique_ptr< BlazarEngine::Graphics::Pass > createPresentPass( );
};