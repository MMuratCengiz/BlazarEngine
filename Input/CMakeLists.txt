INCLUDE(${CMAKE_SOURCE_DIR}/cmake/include_definitions.cmake)

SET(BlazarInputHeaders
        BlazarInput/GlobalEventHandler.h
        BlazarInput/ActionMap.h
        BlazarInput/EventHandler.h
        )

SET(BlazarInputSources
        BlazarInput/GlobalEventHandler.cpp
        BlazarInput/ActionMap.cpp
        BlazarInput/EventHandler.cpp
        )

ADD_LIBRARY(BlazarInput ${BLAZAR_LIB_TYPE} ${BlazarInputHeaders} ${BlazarInputSources})

IF (DEFINED BLAZAR_INSTALL_LIBS)
    INSTALL(TARGETS BlazarInput DESTINATION ${BLAZAR_INSTALL_LOCATION}lib)
    INSTALL(FILES ${BlazarInputHeaders} DESTINATION ${BLAZAR_INSTALL_LOCATION}include)
ENDIF()

SET_TARGET_PROPERTIES(BlazarInput PROPERTIES LINKER_LANGUAGE CXX)

TARGET_INCLUDE_DIRECTORIES(BlazarInput PUBLIC ${CMAKE_SOURCE_DIR}/external/glfw/include)

TARGET_LINK_LIBRARIES(BlazarInput
        glfw
        Vulkan::Vulkan
        BlazarCore
        )