INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/glfw_libs.cmake)

FIND_PACKAGE(glfw3 CONFIG REQUIRED)

SET(BlazarInputSources
        src/BlazarInput/ActionMap.cpp
        src/BlazarInput/EventHandler.cpp
        )

ADD_LIBRARY(BlazarInput ${BLAZAR_LIB_TYPE} ${BlazarInputSources})

TARGET_INCLUDE_DEFAULT_DIRECTORIES(BlazarInput)
TARGET_LINK_LIBRARIES(BlazarInput
        PUBLIC
            BlazarCore glfw
        )

INSTALL_TARGET(BlazarInput)
SET_TARGET_PROPERTIES(BlazarInput PROPERTIES LINKER_LANGUAGE CXX)