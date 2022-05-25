#Sample Configuration:
#-DCMAKE_INSTALL_PREFIX=/Users/crs/Workspace/BlazarEngine/build/BlazarEngine/debug
#-DBLAZAR_INSTALL_LIBS=ON
#-DCMAKE_INSTALL_LIBS=ON

#In OSX Vulkan environment variables should be set:

#VULKAN_SDK=/Users/crs/VulkanSDK/1.3.211.0/macOS
#DYLD_LIBRARY_PATH=${VULKAN_SDK}/lib:${DYLD_LIBRARY_PATH}:
#VK_LAYER_PATH=${VULKAN_SDK/share/vulkan/explicit_layer.d}
#VK_ICD_FILENAMES=${VULKAN_SDK}/share/vulkan/icd.d/MoltenVK_icd.json

CMAKE_MINIMUM_REQUIRED(VERSION 3.20)
PROJECT(BlazarEngine VERSION 0.2)

OPTION(BUILD_SHARED_LIBS OFF)
OPTION(BLAZAR_INSTALL_LIBS ON)
OPTION(BLAZAR_BUILD_AS_LIB OFF)

SET(CPACK_PACKAGE_VENDOR "BlazarGames")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Blazar 3D Game Engine")
SET(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
SET(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

INCLUDE(ExternalProject)

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/include_definitions.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/include_external.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/glfw_libs.cmake)

SET(BLAZAR_LIB_TYPE STATIC)

IF (BUILD_SHARED_LIBS)
    SET(BLAZAR_LIB_TYPE SHARED)
ENDIF()

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
ADD_SHADER(vertex 2d_vertex)
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
ADD_SHADER(fragment 2d_frag)

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

INCLUDE(cmake/common.cmake)
INCLUDE(cmake/include_blazar.cmake)
INCLUDE(cmake/bullet_libs.cmake)

FIND_PACKAGE(Vulkan REQUIRED)

INSTALL(DIRECTORY ${Vulkan_INCLUDE_DIRS}/ DESTINATION include)

INSTALL(FILES ${Vulkan_LIBRARY} DESTINATION lib)
INSTALL(FILES ${Vulkan_glslang_LIBRARY} DESTINATION lib)
INSTALL(FILES ${Vulkan_shaderc_combined_LIBRARY} DESTINATION lib)
INSTALL(FILES ${Vulkan_SPIRV-Tools_LIBRARY} DESTINATION lib)

SET(BLAZAR_LIBS Core ECS Input Graphics Physics Scene Samples)

FOREACH(BLAZAR_L IN LISTS BLAZAR_LIBS)
    ADD_SUBDIRECTORY(${BLAZAR_L})
    INCLUDE_DIRECTORIES(${BLAZAR_L})
    ADD_DEPENDENCIES(Blazar${BLAZAR_L} bullet3 glfw)
ENDFOREACH()

############################################################################################################################################
SET(BlazarRequiredLibs
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
        ${BULLET_LIBS}
        ${GLFW_LIBS}
        )

IF (${BLAZAR_BUILD_AS_LIB})
    ADD_LIBRARY(BlazarEngine ${BLAZAR_LIB_TYPE} main.cpp ${SPVHeaders} ${SPVSources})
ELSE()
    ADD_EXECUTABLE(BlazarEngine main.cpp ${SPVHeaders} ${SPVSources})
ENDIF()

INSTALL(TARGETS BlazarEngine DESTINATION lib)
INSTALL(FILES ${SPVHeaders} DESTINATION include)

SET(BLAZAR_LIBS ${BlazarRequiredLibs} BlazarEngine CACHE INTERNAL "BLAZAR_LIBS")

ADD_DEPENDENCIES(BlazarEngine bullet3)
ADD_DEPENDENCIES(BlazarEngine glfw)

TARGET_COMPILE_DEFINITIONS(BlazarEngine PRIVATE _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)
TARGET_LINK_LIBRARIES(BlazarEngine ${BlazarRequiredLibs})

TARGET_INCLUDE_DIRECTORIES(BlazarEngine PUBLIC ${SPVDir})

INCLUDE(CPack)
