#pragma once

#include <BlazarCore/Common.h>

#include <utility>
#include "../IPipelineProvider.h"
#include "../IResourceProvider.h"

NAMESPACES( ENGINE_NAMESPACE, Graphics )

struct Pass
{
    const std::string name;

    PipelineRequest pipelineRequest; // Todo possibly create multiple
    RenderPassRequest renderPassRequest;

    std::vector< std::string > inputs;
    std::vector< OutputImage > outputs;

    explicit Pass( std::string name ) : name( std::move( name ) )
    { }
};

END_NAMESPACES