INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)

SET(BlazarSamples2DSources
        src/BlazarSamples/SampleGame2D/2DGameScene.cpp
        src/BlazarSamples/SampleGame2D/2DGamePasses.cpp
        src/BlazarSamples/SampleGame2D/2DGameTopDownCamera.cpp)

SET(BlazarSamplesSources
        src/BlazarSamples/FpsCamera.cpp
        src/BlazarSamples/SampleAnimatedFox.cpp
        src/BlazarSamples/SampleGame.cpp
        src/BlazarSamples/SampleGame_Small.cpp
        src/BlazarSamples/SampleAnimatedFox.cpp)

ADD_LIBRARY(BlazarSamples ${BLAZAR_LIB_TYPE} ${BlazarSamplesSources} ${BlazarSamples2DSources})

TARGET_INCLUDE_DEFAULT_DIRECTORIES(BlazarSamples)
TARGET_LINK_LIBRARIES(BlazarSamples
        PUBLIC
            BlazarScene
        )

INSTALL_TARGET(BlazarSamples)
SET_TARGET_PROPERTIES(BlazarSamples PROPERTIES LINKER_LANGUAGE CXX)