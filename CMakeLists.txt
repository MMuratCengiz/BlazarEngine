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

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

INCLUDE(cmake/common.cmake)
INCLUDE(cmake/include_blazar.cmake)

IF(WIN32)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
ELSE()
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
ENDIF()

SET(BLAZAR_LIBS Core ECS Input Graphics Physics Scene Samples)

FOREACH(BLAZAR_L IN LISTS BLAZAR_LIBS)
    ADD_SUBDIRECTORY(${BLAZAR_L})
    TARGET_INCLUDE_DIRECTORIES(Blazar${BLAZAR_L} PUBLIC ${ExternalDependencies_Includes})
    TARGET_LINK_LIBRARIES(Blazar${BLAZAR_L} PUBLIC ${${BLAZAR_L}_LINK_LIBRARIES} ${ExternalDependencies_Libraries})
ENDFOREACH()

############################################################################################################################################
SET(BlazarRequiredLibs
        BlazarCore
        BlazarECS
        BlazarGraphics
        BlazarInput
        BlazarPhysics
        BlazarSamples
        BlazarScene
        ${ExternalDependencies_Libraries}
        )

IF (${BLAZAR_BUILD_AS_LIB})
    ADD_LIBRARY(BlazarEngine ${BLAZAR_LIB_TYPE} main.cpp ${SPVHeaders} ${SPVSources})
    INSTALL(TARGETS BlazarEngine DESTINATION lib)
ELSE()
    ADD_EXECUTABLE(BlazarEngine main.cpp ${SPVHeaders} ${SPVSources})
    INSTALL(TARGETS BlazarEngine DESTINATION bin/..)
ENDIF()

INSTALL(FILES ${SPVHeaders} DESTINATION include)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/install_resources.cmake)

ADD_DEPENDENCIES(BlazarEngine glfw)

FOREACH(BLAZAR_L IN LISTS BLAZAR_LIBS)
    TARGET_INCLUDE_DIRECTORIES(BlazarEngine PUBLIC ${PROJECT_SOURCE_DIR}/${BLAZAR_L})
ENDFOREACH()

TARGET_COMPILE_DEFINITIONS(BlazarEngine PRIVATE _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)
TARGET_LINK_LIBRARIES(BlazarEngine PUBLIC ${BlazarRequiredLibs})

TARGET_INCLUDE_DIRECTORIES(BlazarEngine PUBLIC ${ExternalDependencies_Includes} ${SPVDir})

INCLUDE(CPack)
