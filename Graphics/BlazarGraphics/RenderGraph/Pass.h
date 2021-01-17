#pragma once

#include <BlazarCore/Common.h>

#include <utility>
#include "../IPipelineProvider.h"
#include "../IResourceProvider.h"
#include "GlobalResourceTable.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct Pass
{
    const std::string name;

    std::vector< PipelineRequest > pipelineRequests;
    RenderPassRequest renderPassRequest;

    std::vector< std::vector< std::string > > pipelineInputs;
    std::vector< OutputImage > outputs;

    std::function< int( const std::shared_ptr< ECS::IGameEntity >& entity ) > selectPipeline;

    explicit Pass( std::string name ) : name( std::move( name ) )
    { }
};

END_NAMESPACES