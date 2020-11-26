#pragma once

#include "../Core/Common.h"
#include "../ECS/IGameEntity.h"
#include "DescriptorManager.h"

NAMESPACES( SomeVulkan, Graphics )

struct PipelineInstance {
    std::string name;
    std::shared_ptr< DescriptorManager > descriptorManager;
    vk::Pipeline pipeline;
    vk::PipelineLayout layout;
};

typedef std::function< std::string( const std::shared_ptr< ECS::IGameEntity > & ) > PipelineSelectorFunc;

struct PipelineSelectorPair {
    uint32_t priority = 0;
    PipelineSelectorFunc selectorFunc;
};

class PipelineSelector {
private:
    std::vector< PipelineInstance > instances;
    std::unordered_map< std::string, PipelineInstance > instanceMap;
    std::vector< PipelineSelectorPair > selectors;
    std::vector< std::type_index > pipelineComponents;
public:
    void createPipelineInstance( PipelineInstance &instance ) {
        instanceMap[ instance.name ] = instance;
        instances.emplace_back( instance );
    }

    void addSelector( const PipelineSelectorPair selectorPair ) {
        selectors.emplace_back( selectorPair );
    }

    const std::vector< PipelineInstance >& selectAll() {
        return instances;
    }

    PipelineInstance &selectPipeline( const std::shared_ptr< ECS::IGameEntity > &entity ) {
        sortSelectors();

        for ( PipelineSelectorPair& selector: selectors ) {
            std::string supported = selector.selectorFunc( entity );

            if ( !supported.empty() ) {
                return instanceMap[ supported ];
            }
        }

        throw std::runtime_error( "No matching pipeline found!" );
    }

private:
    void sortSelectors( ) {
        const auto &comparator = [ ]( const PipelineSelectorPair &p1, const PipelineSelectorPair &p2 ) -> bool {
            return p1.priority > p2.priority;
        };

        std::sort( selectors.begin( ), selectors.end( ), comparator );
    }
};

END_NAMESPACES
