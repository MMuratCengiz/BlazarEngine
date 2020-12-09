SET(BlazarSamplesSources
        BlazarSamples/FpsCamera.h
        BlazarSamples/FpsCamera.cpp
        BlazarSamples/SampleCrate.h
        BlazarSamples/SampleSetupInputBindings.h
        BlazarSamples/SampleSmallCrate.h
        BlazarSamples/SampleAnimatedWolf.h
        BlazarSamples/SampleBall.h
        BlazarSamples/SampleGame.h
        BlazarSamples/SampleGame.cpp
        BlazarSamples/SampleHouse.h
        BlazarSamples/SampleFloor.h
        BlazarSamples/SampleTrafficCone.h
        BlazarSamples/SampleCar1.h
        BlazarSamples/SampleCar2.h
        BlazarSamples/SampleOldHouse.h
        BlazarSamples/SampleCubeMap.h
        BlazarSamples/Spaceship.h
        )

ADD_LIBRARY(BlazarSamples ${BlazarSamplesSources})

SET_TARGET_PROPERTIES(BlazarSamples PROPERTIES LINKER_LANGUAGE CXX)

TARGET_LINK_LIBRARIES(
        BlazarSamples
        BlazarECS
        BlazarInput
        BlazarScene
        BlazarGraphics
        BlazarPhysics
)