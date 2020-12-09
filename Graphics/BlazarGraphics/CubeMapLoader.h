#pragma once

#include <BlazarECS/ECS.h>
#include "InstanceContext.h"
#include "CommandExecutor.h"
#include "RenderUtilities.h"
#include "MaterialLoader.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

class CubeMapLoader
{
private:
    std::unordered_map< std::string, TextureBuffer > loadedCubeMaps;
    std::shared_ptr< InstanceContext > context;
    std::shared_ptr< CommandExecutor > commandExecutor;
public:
    inline explicit CubeMapLoader( std::shared_ptr< InstanceContext > context, std::shared_ptr< CommandExecutor > commandExecutor )
            : context( std::move( context ) ), commandExecutor( std::move( commandExecutor ) )
    {
    }

    void cache( ECS::CCubeMap &material );
    void load( TextureBuffer &input, ECS::CCubeMap &material );
    static std::string getKey( const ECS::CCubeMap &material );
    ~CubeMapLoader( );
private:
    void loadInner( ECS::CCubeMap &material );
};

END_NAMESPACES