INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)

SET(BlazarSceneHeaders
        BlazarScene/World.h
        BlazarScene/Scene.h
        BlazarScene/FPSCounter.h
        BlazarScene/IPlayable.h
        BlazarScene/Window.h
        )

SET(BlazarSceneSources
        BlazarScene/World.cpp
        )

ADD_LIBRARY(BlazarScene ${BLAZAR_LIB_TYPE} ${BlazarSceneHeaders} ${BlazarSceneSources})

IF (BLAZAR_INSTALL_LIBS)
    INSTALL_TARGET(BlazarScene)
    INSTALL(FILES ${BlazarSceneHeaders} DESTINATION include/BlazarScene)
ENDIF()

SET_TARGET_PROPERTIES(BlazarScene PROPERTIES LINKER_LANGUAGE CXX)

SET(INCLUDE_BOOST_TARGET BlazarScene)
SET(REQUESTED_BOOST_LIBS thread format atomic)

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/include_boost.cmake)

TARGET_INCLUDE_DIRECTORIES(BlazarScene PUBLIC
        ${PROJECT_SOURCE_DIR}/external/glfw/include
        ${PROJECT_SOURCE_DIR}/Core
        ${PROJECT_SOURCE_DIR}/ECS
        ${PROJECT_SOURCE_DIR}/Input
        ${PROJECT_SOURCE_DIR}/Physics
        ${PROJECT_SOURCE_DIR}/Graphics
        )

SET(Scene_LINK_LIBRARIES
        BlazarCore
        BlazarECS
        BlazarInput
        BlazarPhysics
        BlazarGraphics
        Vulkan::Vulkan
        PARENT_SCOPE)
