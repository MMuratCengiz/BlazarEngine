#pragma once

#include "../core/Common.h"
#include <fstream>

NAMESPACES( SomeVulkan, Graphics )

enum class ShaderType {
    Vertex,
    Fragment
};

struct Shader {
    ShaderType type;
    std::string filename;
};

class GLSLShader {
private:
    std::vector< char > contents;
public:
    GLSLShader( ShaderType type, const std::string& path );
private:
    void readFile( const std::string &filename );
};

END_NAMESPACES
