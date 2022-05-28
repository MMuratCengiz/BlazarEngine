INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/glfw_libs.cmake)

SET(BlazarInputHeaders
        BlazarInput/GlobalEventHandler.h
        BlazarInput/ActionMap.h
        BlazarInput/EventHandler.h
        )

SET(BlazarInputSources
        BlazarInput/ActionMap.cpp
        BlazarInput/EventHandler.cpp
        )

ADD_LIBRARY(BlazarInput ${BLAZAR_LIB_TYPE} ${BlazarInputHeaders} ${BlazarInputSources})

IF (BLAZAR_INSTALL_LIBS)
    INSTALL_TARGET(BlazarInput)
    INSTALL(FILES ${BlazarInputHeaders} DESTINATION include/BlazarInput)
ENDIF()

SET_TARGET_PROPERTIES(BlazarInput PROPERTIES LINKER_LANGUAGE CXX)

TARGET_INCLUDE_DIRECTORIES(BlazarInput PUBLIC
        ${PROJECT_SOURCE_DIR}/external/glfw/include
        ${PROJECT_SOURCE_DIR}/Core
        )

SET(Input_LINK_LIBRARIES BlazarCore PARENT_SCOPE)