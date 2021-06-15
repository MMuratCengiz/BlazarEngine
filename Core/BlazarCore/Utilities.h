#pragma once

#include "Common.h"

NAMESPACES( ENGINE_NAMESPACE, Core )

class Utilities
{
private:
    Utilities() = default;
public:
    static glm::vec3 quatToEulerGlm( const btQuaternion& quat3 );
    static glm::vec3 toGlm( const btVector3& vec3 );
    static glm::vec4 toGlm( const btVector4& vec4 );
    static btVector3 toBt( glm::vec3 vec );
    static btQuaternion toBtQuat( glm::vec3 euler, bool isInRadians );
    static std::vector< char > readFile( const std::string &filename );
    static glm::mat4 getTRSMatrix( const glm::vec3& t, const glm::quat& r, const glm::vec3& s );
};

END_NAMESPACES
