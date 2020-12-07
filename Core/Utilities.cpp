#include "Utilities.h"

NAMESPACES( ENGINE_NAMESPACE, Core )

glm::vec3 Utilities::quatToEulerGlm( const btQuaternion &quat3 )
{
    glm::vec3 result;

    quat3.getEulerZYX( result.z, result.y, result.x );

    return result;
}

glm::vec3 Utilities::toGlm( const btVector3 &vec3 )
{
    return glm::vec3(
            vec3.getX(),
            vec3.getY(),
            vec3.getZ()
            );
}

glm::vec4 Utilities::toGlm( const btVector4 &vec4 )
{
    return glm::vec4(
            vec4.getX(),
            vec4.getY(),
            vec4.getZ(),
            vec4.getW()
    );
}

btVector3 Utilities::toBt( glm::vec3 vec )
{
    return btVector3
    {
        vec.x,
        vec.y,
        vec.z
    };
}

btQuaternion Utilities::toBtQuat( ECS::Rotation rotation )
{
    btQuaternion quaternion{ };

    if ( rotation.rotationUnit == ECS::RotationUnit::Radians )
    {
        quaternion.setEuler(
            rotation.euler.x,
            rotation.euler.y,
            rotation.euler.z
        );
    }
    else
    {
        quaternion.setEuler(
                glm::radians( rotation.euler.x ),
                glm::radians( rotation.euler.y ),
                glm::radians( rotation.euler.z )
        );
    }

    return btQuaternion( );
}

END_NAMESPACES

