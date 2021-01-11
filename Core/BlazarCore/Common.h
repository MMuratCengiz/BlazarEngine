#pragma once

#define NAMESPACES( N1, N2 ) namespace N1 { namespace N2 {
#define NAMESPACE( N1 ) namespace N1 {
#define END_NAMESPACES } }
#define END_NAMESPACE }

#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <stdlib.h>
#include <malloc.h>
#include <string>
#include <iostream>
#include <sstream>
#include <functional>
#include <memory>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <cstring>
#include "Time.h"
#include <stb_image.h>
#include "btBulletDynamicsCommon.h"

#define PATH( P ) std::string(ROOT_DIR) + P

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
#define COMPONENT_GRAPHICS "Graphics"
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

#define while_false( statement ) do { statement } while ( false )
#define ASSERT_M( val, message ) while_false( if ( !( val ) ) { throw std::runtime_error( message ); } )
#define ASSERT( val ) ASSERT_M( val, "assert val failed!" )

#define NOT_NULL( val ) ASSERT_M( val != nullptr, "val cannot be null!" )
#define VkCheckResult( R ) ASSERT( R == vk::Result::eSuccess )
#define IS_NULL( val ) ( val == nullptr )
#define FUNCTION_BREAK( condition ) if ( condition ) return;

#define VK_CORRECTION_MATRIX glm::mat4(  1.0f,  0.0f, 0.0f, 0.0f, \
                                         0.0f, -1.0f, 0.0f, 0.0f, \
                                         0.0f,  0.0f, 0.5f, 0.0f, \
                                         0.0f,  0.0f, 0.5f, 1.0f)

#include "Constants.h"

//#define ENABLE_SHADOW_DEBUG_OUTPUT