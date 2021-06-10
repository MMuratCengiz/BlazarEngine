INCLUDE(${CMAKE_SOURCE_DIR}/cmake/include_definitions.cmake)

SET(BlazarSceneSources
        BlazarScene/World.h
        BlazarScene/Scene.h
        BlazarScene/FPSCounter.h
        BlazarScene/IPlayable.h
        BlazarScene/Window.h
        BlazarScene/World.cpp)

ADD_LIBRARY(BlazarScene ${BlazarSceneSources})

SET_TARGET_PROPERTIES(BlazarScene PROPERTIES LINKER_LANGUAGE CXX)

TARGET_INCLUDE_DIRECTORIES(BlazarScene PUBLIC ${CMAKE_SOURCE_DIR}/external/glfw/include)

TARGET_LINK_LIBRARIES(BlazarScene
        Vulkan::Vulkan
        )

