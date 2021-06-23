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

    // SMAA passes
    static std::shared_ptr< Pass > createSMAAEdgePass( IRenderDevice* renderDevice );
    static std::shared_ptr< Pass > createSMAABlendWeightPass( IRenderDevice* renderDevice );
    static std::shared_ptr< Pass > createSMAANeighborPass( IRenderDevice* renderDevice );
};

END_NAMESPACES

