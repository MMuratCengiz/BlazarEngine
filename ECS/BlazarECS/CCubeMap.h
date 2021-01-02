#pragma once

#include <BlazarCore/Common.h>
#include "IComponent.h"

NAMESPACES( ENGINE_NAMESPACE, ECS )

/*
 * Single vector value means the texture contains all six images, otherwise there must! be exactly 6 images for each face of the cube.
 */

enum class CubeMapSide : uint32_t
{
    Single = 0,
    Right = 6,
    Left = 5,
    Top = 4,
    Bottom = 3,
    Front = 2,
    Back = 1
};

struct CubeMapSidePath
{
    CubeMapSide side;
    std::string path;
};

struct CCubeMap : public IComponent
{
public:
    std::vector< CubeMapSidePath > texturePaths;
    BLAZAR_COMPONENT( CCubeMap )

    inline void sort( )
    {
        std::sort( texturePaths.begin( ), texturePaths.end( ), [ ]( const ECS::CubeMapSidePath &p1, const ECS::CubeMapSidePath &p2 ) -> bool
        {
            return static_cast< uint32_t >( p1.side ) > static_cast< uint32_t >( p2.side );
        } );
    }
};

END_NAMESPACES