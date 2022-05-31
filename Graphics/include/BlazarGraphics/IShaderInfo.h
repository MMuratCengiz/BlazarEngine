#pragma once

#include <BlazarCore/Common.h>

NAMESPACES(ENGINE_NAMESPACE, Graphics)

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

class IShaderInfo
{
public:
    virtual std::vector< std::string > getMergedInputs( ) = 0;
    virtual ~IShaderInfo( ) = default;
};

END_NAMESPACES