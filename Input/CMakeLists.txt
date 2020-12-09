SET(BlazarInputSources
        BlazarInput/GlobalEventHandler.h
        BlazarInput/GlobalEventHandler.cpp
        BlazarInput/ActionMap.cpp
        BlazarInput/ActionMap.h
        BlazarInput/EventHandler.cpp
        BlazarInput/EventHandler.h
        )

ADD_LIBRARY(BlazarInput ${BlazarInputSources})

SET_TARGET_PROPERTIES(BlazarInput PROPERTIES LINKER_LANGUAGE CXX)

TARGET_INCLUDE_DIRECTORIES(BlazarInput PUBLIC ${CMAKE_SOURCE_DIR}/external/glfw/include)

TARGET_LINK_LIBRARIES(BlazarInput
        glfw
        Vulkan::Vulkan
        BlazarCore
        )