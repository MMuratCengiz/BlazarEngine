INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)

SET(BlazarSceneSources src/BlazarScene/World.cpp)

ADD_LIBRARY(BlazarScene ${BLAZAR_LIB_TYPE} ${BlazarSceneSources})

TARGET_INCLUDE_DEFAULT_DIRECTORIES(BlazarScene)
TARGET_LINK_LIBRARIES(BlazarScene
        PUBLIC
            BlazarPhysics
            BlazarGraphics
        )

INSTALL_TARGET(BlazarScene)
SET_TARGET_PROPERTIES(BlazarScene PROPERTIES LINKER_LANGUAGE CXX)