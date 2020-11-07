#pragma once

#define NAMESPACES( N1, N2 ) namespace N1 { namespace N2 {
#define END_NAMESPACES } }

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <string>
#include <iostream>
#include <sstream>
#include <functional>
#include <assert.h>
#include <memory>
#include <glm/glm.hpp>
#include <cstring>
#include "Time.h"
#include "../external/loaders/stb_image.h"

typedef struct StatusInfo {
    int code;
    std::string message;
} StatusInfo;

#define PATH(P) std::string(ROOT_DIR) + P

#ifdef DEBUG
#define TRACE( COMPONENT, VERBOSITY, MSG )                              \
    if ( VERBOSITY <= G_VERBOSITY ) {                                   \
        fprintf( stdout, "[%s][%d]: %s\n", COMPONENT, VERBOSITY, MSG ); \
    }
#elif
#define TRACE( COMPONENT, VERBOSITY, MSG )
#define TRACE_STATUS( COMPONENT, VERBOSITY, STATUS )
#endif

#define COMPONENT_VKAPI "RenderDevice"
#define COMPONENT_VKPRESENTATION "RenderSurface"
#define COMPONENT_GAMEH "GAMEH"
#define COMPONENT_TLOAD "TEXTURELOADING"

#define VERBOSITY_CRITICAL 1
#define VERBOSITY_HIGH 2
#define VERBOSITY_INFORMATION 3
#define VERBOSITY_LOW 4

#define SFORMAT( COUNT, F, OUT )    \
    std::stringstream sstreamCOUNT; \
    sstreamCOUNT << F;              \
    OUT = sstreamCOUNT.str()

#define ENTITY_CAST( instance ) std::dynamic_pointer_cast< IGameEntity >( instance )

class Utilities {
public:
    template < class Type >
    static Type matchAndGetOrDefault( Type def, std::vector< Type > list, std::function< bool( Type t ) > condition) {
        for ( Type t : list ) {
            if ( condition( t ) ) {
                return t;
            }
        }

        return def;
    }
};

#define range( var, from, to ) for ( uint32_t var = from; from < to; ++var )
#define until( var, to ) for ( uint32_t var = 0; var < to; ++var )

#define VkCheckResult( R ) assert( R == vk::Result::eSuccess )