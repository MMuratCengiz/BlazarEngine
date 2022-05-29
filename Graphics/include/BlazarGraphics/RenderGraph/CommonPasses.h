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
    static std::unique_ptr< Pass > createGBufferPass( );
    static std::unique_ptr< Pass > createShadowMapPass( );
    static std::unique_ptr< Pass > createLightingPass( );
    static std::unique_ptr< Pass > createSkyBoxPass( );
    static std::unique_ptr< Pass > createPresentPass( );

    // SMAA passes
    static std::unique_ptr< Pass > createSMAAEdgePass( );
    static std::unique_ptr< Pass > createSMAABlendWeightPass( );
    static std::unique_ptr< Pass > createSMAANeighborPass( );
};

END_NAMESPACES

