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

#include <utility>
#include "../IPipelineProvider.h"
#include "../IResourceProvider.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

enum class InputGeometry
{
    Model,
    Quad,
    Cube,
    OverSizedTriangle
};

struct Pass
{
    const std::string name;

    InputGeometry inputGeometry;
    std::vector< PipelineRequest > pipelineRequests;
    RenderPassRequest renderPassRequest;
    std::vector< OutputImage > outputs;

    // If more than one pipelines are returned the same object is rendered multiple times with different pipelines
    std::function< std::vector< int >( ECS::IGameEntity * entity ) > selectPipeline;

    explicit Pass( std::string name ) : name( std::move( name ) )
    { }
};

END_NAMESPACES