INCLUDE(${CMAKE_SOURCE_DIR}/cmake/include_definitions.cmake)

SET(BlazarSamplesSources
        BlazarSamples/FpsCamera.h
        BlazarSamples/FpsCamera.cpp
        BlazarSamples/SampleAnimatedFox.h
        BlazarSamples/SampleAnimatedFox.cpp
        BlazarSamples/SampleCrate.h
        BlazarSamples/SampleSetupInputBindings.h
        BlazarSamples/SampleSmallCrate.h
        BlazarSamples/SampleGame.h
        BlazarSamples/SampleGame.cpp
        BlazarSamples/SampleGame_Small.h
        BlazarSamples/SampleGame_Small.cpp
        BlazarSamples/SampleFloor.h
        BlazarSamples/SampleCar1.h
        BlazarSamples/SampleCar2.h
        BlazarSamples/SampleCubeMap.h
        BlazarSamples/SampleMovingCrate.h BlazarSamples/SampleAnimatedFox.cpp)

ADD_LIBRARY(BlazarSamples ${BlazarSamplesSources})

SET(INCLUDE_BOOST_TARGET BlazarSamples)

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/include_boost.cmake)

SET_TARGET_PROPERTIES(BlazarSamples PROPERTIES LINKER_LANGUAGE CXX)

TARGET_LINK_LIBRARIES(
        BlazarSamples
        BlazarECS
        BlazarInput
        BlazarScene
        BlazarGraphics
        BlazarPhysics
)