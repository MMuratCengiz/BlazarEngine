CMAKE_MINIMUM_REQUIRED(VERSION 3.19)

PROJECT(BlazarEngine)

SET(SHADER_ROOT_DIR ${CMAKE_SOURCE_DIR}/Shaders/)

FUNCTION(add_shader SHADER_TYPE SHADER_NAME)
    SET(INPUT_FILE "${SHADER_ROOT_DIR}GLSL/${SHADER_TYPE}/${SHADER_NAME}.glsl")
    SET(OUTPUT_FILE "${SHADER_ROOT_DIR}SPIRV/${SHADER_TYPE}/${SHADER_NAME}.spv")

    #remember to add glslc to environment path
    EXECUTE_PROCESS(COMMAND glslc -fshader-stage=${SHADER_TYPE} ${INPUT_FILE} -o ${OUTPUT_FILE})
ENDFUNCTION()

ADD_SHADER(vertex cube_position)
ADD_SHADER(fragment skybox_default)
ADD_SHADER(fragment present_pass)
ADD_SHADER(fragment lighting_pass)
ADD_SHADER(fragment skybox_pass)
ADD_SHADER(vertex gBuffer)
ADD_SHADER(vertex gBuffer_outlined)
ADD_SHADER(vertex gBuffer_Animated)
ADD_SHADER(vertex tessellated_proxy)

ADD_SHADER(fragment gBuffer)
ADD_SHADER(fragment gBuffer_outlined)
ADD_SHADER(vertex shadowMap)
ADD_SHADER(tesscontrol height_map)
ADD_SHADER(tesseval height_map)

# SMAA SHADERS:
ADD_SHADER(vertex smaaEdge)
ADD_SHADER(fragment smaaEdge)
ADD_SHADER(vertex smaaNeighbor)
ADD_SHADER(fragment smaaNeighbor)
ADD_SHADER(vertex smaaBlendWeight)
ADD_SHADER(fragment smaaBlendWeight)
# //---

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/include_definitions.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/include_external.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/include_blazar.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/include_bullet.cmake)

ADD_DEFINITIONS(-DENGINE_NAMESPACE=BlazarEngine)
ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ADD_DEFINITIONS(-DDEBUG)
ADD_DEFINITIONS(-DG_VERBOSITY=3)
ADD_DEFINITIONS(-DROOT_DIR="${CMAKE_SOURCE_DIR}")
ADD_DEFINITIONS(-DGTEST_LANG_CXX11=1)

FIND_PACKAGE(Vulkan REQUIRED)

# BLAZAR CORE LIBRARY #####################################################################################################################
ADD_SUBDIRECTORY("${CMAKE_SOURCE_DIR}/Core/")

INCLUDE_DIRECTORIES(
        "${CMAKE_SOURCE_DIR}/Core"
)
###########################################################################################################################################

# BLAZAR ECS LIBRARY ######################################################################################################################
ADD_SUBDIRECTORY("${CMAKE_SOURCE_DIR}/ECS/")

INCLUDE_DIRECTORIES(
        "${CMAKE_SOURCE_DIR}/ECS"
)
############################################################################################################################################

# BLAZAR INPUT LIBRARY #####################################################################################################################
ADD_SUBDIRECTORY("${CMAKE_SOURCE_DIR}/Input/")

INCLUDE_DIRECTORIES(
        "${CMAKE_SOURCE_DIR}/Input"
)
############################################################################################################################################

# BLAZAR GRAPHICS LIBRARY ##################################################################################################################
ADD_SUBDIRECTORY("${CMAKE_SOURCE_DIR}/Graphics/")

INCLUDE_DIRECTORIES(
        "${CMAKE_SOURCE_DIR}/Graphics"
)
############################################################################################################################################

# BLAZAR PHYSICS LIBRARY ###################################################################################################################
ADD_SUBDIRECTORY("${CMAKE_SOURCE_DIR}/Physics/")

INCLUDE_DIRECTORIES(
        "${CMAKE_SOURCE_DIR}/Physics"
)
############################################################################################################################################

# BLAZAR SCENE LIBRARY #####################################################################################################################
ADD_SUBDIRECTORY("${CMAKE_SOURCE_DIR}/Scene/")

INCLUDE_DIRECTORIES(
        "${CMAKE_SOURCE_DIR}/Scene"
)
############################################################################################################################################

# BLAZAR SAMPLES LIBRARY ###################################################################################################################
ADD_SUBDIRECTORY("${CMAKE_SOURCE_DIR}/Samples/")

INCLUDE_DIRECTORIES(
        "${CMAKE_SOURCE_DIR}/Samples"
)
############################################################################################################################################

ADD_EXECUTABLE(BlazarEngine main.cpp ${Core} ${Graphics} ${Physics} ${Input} ${ECS} ${RenderObjects} ${Scene} ${Samples} ${SPV})

TARGET_COMPILE_DEFINITIONS(BlazarEngine PRIVATE _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)
TARGET_LINK_LIBRARIES(BlazarEngine
        glm
        Vulkan::Vulkan
# Blazar Libraries
        BlazarCore
        BlazarECS
        BlazarGraphics
        BlazarInput
        BlazarPhysics
        BlazarSamples
        BlazarScene
# -- Blazar Libraries
# Bullet Libraries
        Bullet3Common
        Bullet3Geometry
        BulletSoftBody
        BulletDynamics
        Bullet3Dynamics
        Bullet3Collision
        BulletCollision
        LinearMath
        BulletInverseDynamics
# -- Bullet Libraries
        )

TARGET_INCLUDE_DIRECTORIES(BlazarEngine PUBLIC ${SPVDir})
