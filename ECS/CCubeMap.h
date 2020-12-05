#pragma once

#include "../Core/Common.h"
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

/*
 * Single vector value means the texture contains all six images, otherwise there must! be exactly 6 images for each face of the cube.
 */

enum class CubeMapSide : uint32_t {
    Single = 0,
    Right = 6,
    Left = 5,
    Top = 4,
    Bottom = 3,
    Front = 2,
    Back = 1
};

struct CubeMapSidePath {
    CubeMapSide side;
    std::string path;
};

struct CCubeMap: public IComponent {
    std::vector< CubeMapSidePath > texturePaths;

};

END_NAMESPACES