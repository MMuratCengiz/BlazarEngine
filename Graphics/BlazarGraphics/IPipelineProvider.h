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
    Less,
    LessOrEqual,
    Greater,
    GreaterOrEqual
};

enum class BindPoint
{
    Graphics,
    Compute
};

struct PipelineRequest
{
    ECS::CullMode cullMode;
    CompareOp depthCompareOp;

    std::unordered_map< ShaderType, std::string > shaderPaths;

    std::shared_ptr< IRenderPass > parentPass;
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
