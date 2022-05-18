CMAKE_MINIMUM_REQUIRED(VERSION 3.19)

INCLUDE(ExternalProject)

#hardcoded for now
SET(VULKAN_SDK /Users/crs/VulkanSDK/1.3.204.1/macOS)
SET(DYLD_LIBRARY_PATH ${VULKAN_SDK}/lib:${DYLD_LIBRARY_PATH}:)
SET(VK_LAYER_PATH ${VULKAN_SDK/share/vulkan/explicit_layer.d})
SET(VK_ICD_FILENAMES ${VULKAN_SDK}/share/vulkan/icd.d/MoltenVK_icd.json)

OPTION(BLAZAR_INSTALL_LIBS OFF)
OPTION(BLAZAR_BUILD_SHARED_LIBS OFF)

SET(BLAZAR_LIB_TYPE STATIC)
IF (BLAZAR_BUILD_SHARED_LIBS)
    SET(BLAZAR_LIB_TYPE SHARED)
ENDIF()

PROJECT(BlazarEngine VERSION 0.2)

SET(SHADER_ROOT_DIR ${PROJECT_SOURCE_DIR}/Shaders/)

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
ADD_SHADER(vertex quad_position)

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

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/include_definitions.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/include_external.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/include_blazar.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/include_bullet.cmake)

FIND_PACKAGE(Vulkan REQUIRED)

# BLAZAR CORE LIBRARY #####################################################################################################################
ADD_SUBDIRECTORY("${PROJECT_SOURCE_DIR}/Core/")

INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/Core"
)
###########################################################################################################################################

# BLAZAR ECS LIBRARY ######################################################################################################################
ADD_SUBDIRECTORY("${PROJECT_SOURCE_DIR}/ECS/")

INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/ECS"
)
############################################################################################################################################

# BLAZAR INPUT LIBRARY #####################################################################################################################
ADD_SUBDIRECTORY("${PROJECT_SOURCE_DIR}/Input/")

INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/Input"
)
############################################################################################################################################

# BLAZAR GRAPHICS LIBRARY ##################################################################################################################
ADD_SUBDIRECTORY("${PROJECT_SOURCE_DIR}/Graphics/")

INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/Graphics"
)
############################################################################################################################################

# BLAZAR PHYSICS LIBRARY ###################################################################################################################
ADD_SUBDIRECTORY("${PROJECT_SOURCE_DIR}/Physics/")

INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/Physics"
)
############################################################################################################################################

# BLAZAR SCENE LIBRARY #####################################################################################################################
ADD_SUBDIRECTORY("${PROJECT_SOURCE_DIR}/Scene/")

INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/Scene"
)
############################################################################################################################################

# BLAZAR SAMPLES LIBRARY ###################################################################################################################
ADD_SUBDIRECTORY("${PROJECT_SOURCE_DIR}/Samples/")

INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/Samples"
)
############################################################################################################################################
SET(BlazarRequiredLibs
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
        glm
        glfw
        ${LUA_LIB}
        )

IF (${BLAZAR_INSTALL_LIBS})
    ADD_LIBRARY(BlazarEngine ${BLAZAR_LIB_TYPE} main.cpp ${SPVHeaders} ${SPVSources})
    ADD_DEPENDENCIES(BlazarEngine lua)

    INSTALL(TARGETS BlazarEngine DESTINATION ${BLAZAR_INSTALL_LOCATION}lib)
    INSTALL(FILES ${SPVHeaders} DESTINATION ${BLAZAR_INSTALL_LOCATION}include)

    SET(BLAZAR_INCLUDE_DIRS ${BlazarSources} CACHE INTERNAL "BLAZAR_INCLUDE_DIRS")
    SET(BLAZAR_LIBS ${BlazarRequiredLibs} BlazarEngine CACHE INTERNAL "BLAZAR_LIBS")
ELSE()
    ADD_EXECUTABLE(BlazarEngine main.cpp ${SPVHeaders} ${SPVSources})
    ADD_DEPENDENCIES(BlazarEngine lua)
    TARGET_INCLUDE_DIRECTORIES(BlazarEngine PRIVATE ${SOURCE_DIR}/src)
ENDIF()

TARGET_COMPILE_DEFINITIONS(BlazarEngine PRIVATE _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)
TARGET_LINK_LIBRARIES(BlazarEngine ${BlazarRequiredLibs})

TARGET_INCLUDE_DIRECTORIES(BlazarEngine PUBLIC ${SPVDir})
