#pragma once

#include <BlazarCore/Common.h>
#include <BlazarECS/ECS.h>
#include "IRenderPassProvider.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class IRenderPass;

enum class ShaderType
{
    Vertex,
    Fragment
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

struct EnabledPipelineStages
{
    bool vertex : 1;
    bool fragment : 1;
};

struct PipelineRequest
{
    ECS::CullMode cullMode;
    CompareOp depthCompareOp;

    EnabledPipelineStages enabledPipelineStages;
    std::string vertexShaderPath;
    std::string fragmentShaderPath;

    std::shared_ptr< IRenderPass > parentPass;
};

struct IPipeline
{
    std::string name;
    BindPoint bindPoint;
    virtual ~IPipeline( ) = default;
};

class IPipelineProvider
{
public:
    virtual std::shared_ptr< IPipeline > createPipeline( const PipelineRequest& request ) = 0;
    virtual ~IPipelineProvider( ) = default;
};

END_NAMESPACES
