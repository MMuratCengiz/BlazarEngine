INCLUDE(${PROJECT_SOURCE_DIR}/cmake/include_definitions.cmake)

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

IF (DEFINED BLAZAR_INSTALL_LIBS)
    INSTALL(TARGETS BlazarScene DESTINATION ${BLAZAR_INSTALL_LOCATION}lib)
    INSTALL(FILES ${BlazarSceneHeaders} DESTINATION ${BLAZAR_INSTALL_LOCATION}include/BlazarScene)
    LIST(APPEND BlazarSources ${PROJECT_SOURCE_DIR}/BlazarScene)
ENDIF()

SET_TARGET_PROPERTIES(BlazarScene PROPERTIES LINKER_LANGUAGE CXX)

SET(INCLUDE_BOOST_TARGET BlazarScene)
SET(REQUESTED_BOOST_LIBS thread format atomic)

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/include_boost.cmake)

TARGET_INCLUDE_DIRECTORIES(BlazarScene PUBLIC ${PROJECT_SOURCE_DIR}/external/glfw/include)

TARGET_LINK_LIBRARIES(BlazarScene
        Vulkan::Vulkan
        )

