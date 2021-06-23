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
#include <BlazarECS/ECS.h>
#include "IRenderPassProvider.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class IRenderPass;

enum class ShaderType
{
    Vertex,
    Fragment,
    TessellationEval,
    TessellationControl,
    Geometry
};

struct Shader
{
    ShaderType type;
    std::string path;
};

enum class CompareOp
{
    Equal,
    NotEqual,
    Always,
    Less,
    LessOrEqual,
    Greater,
    GreaterOrEqual
};

enum class StencilOp
{
    Keep,
    Zero,
    Replace,
    IncrementAndClamp,
    DecrementAndClamp,
    Invert,
    IncrementAndWrap,
    DecrementAndWrap
};

struct StencilTestState
{
    bool enabled = false;
    CompareOp compareOp;
    uint32_t compareMask;
    uint32_t writeMask;
    uint32_t ref;

    StencilOp failOp;
    StencilOp passOp;
    StencilOp depthFailOp;
};

enum class BindPoint
{
    Graphics,
    Compute
};

enum class BlendMode
{
    None,
    AlphaBlend
};

struct PipelineRequest
{
    ECS::CullMode cullMode;
    CompareOp depthCompareOp;
    bool enableDepthTest = true;

    StencilTestState stencilTestStateFront { };
    StencilTestState stencilTestStateBack { };

    std::unordered_map< ShaderType, std::string > shaderPaths;

    std::shared_ptr< IRenderPass > parentPass;
    BlendMode blendMode = BlendMode::None;
};

struct IPipeline
{
    std::string name;
    BindPoint bindPoint;
    virtual void cleanup( ) = 0;
    virtual ~IPipeline( ) = default;
};

class IPipelineProvider
{
public:
    virtual std::shared_ptr< IPipeline > createPipeline( const PipelineRequest& request ) = 0;
    virtual ~IPipelineProvider( ) = default;
};

END_NAMESPACES
