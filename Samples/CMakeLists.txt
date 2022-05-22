INCLUDE(${PROJECT_SOURCE_DIR}/cmake/include_definitions.cmake)


SET(BlazarSamples2DHeaders
        BlazarSamples/SampleGame2D/2DGameScene.h BlazarSamples/SampleGame2D/2DGamePasses.h
        BlazarSamples/SampleGame2D/2DGameTopDownCamera.h)

SET(BlazarSamples2DSources
        BlazarSamples/SampleGame2D/2DGameScene.cpp BlazarSamples/SampleGame2D/2DGamePasses.cpp BlazarSamples/SampleGame2D/2DGameTopDownCamera.cpp)

SET(BlazarSamplesHeaders
        BlazarSamples/FpsCamera.h
        BlazarSamples/SampleAnimatedFox.h
        BlazarSamples/SampleCrate.h
        BlazarSamples/SampleSetupInputBindings.h
        BlazarSamples/SampleSmallCrate.h
        BlazarSamples/SampleGame.h
        BlazarSamples/SampleGame_Small.h
        BlazarSamples/SampleFloor.h
        BlazarSamples/SampleCar1.h
        BlazarSamples/SampleCar2.h
        BlazarSamples/SampleCubeMap.h
        BlazarSamples/SampleMovingCrate.h
        )

SET(BlazarSamplesSources
        BlazarSamples/FpsCamera.cpp
        BlazarSamples/SampleAnimatedFox.cpp
        BlazarSamples/SampleGame.cpp
        BlazarSamples/SampleGame_Small.cpp
        BlazarSamples/SampleAnimatedFox.cpp)

ADD_LIBRARY(BlazarSamples ${BLAZAR_LIB_TYPE} ${BlazarSamples2DHeaders} ${BlazarSamplesHeaders} ${BlazarSamplesSources} ${BlazarSamples2DSources})

IF (${BLAZAR_INSTALL_LIBS})
    INSTALL(TARGETS BlazarSamples DESTINATION ${BLAZAR_INSTALL_LOCATION}lib)
    INSTALL(FILES ${BlazarSamplesHeaders} DESTINATION ${BLAZAR_INSTALL_LOCATION}include/BlazarSamples)
    INSTALL(FILES ${BlazarSamples2DHeaders} DESTINATION ${BLAZAR_INSTALL_LOCATION}include/BlazarSamples/SampleGame2D)
ENDIF()

SET(INCLUDE_BOOST_TARGET BlazarSamples)

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/include_boost.cmake)

SET_TARGET_PROPERTIES(BlazarSamples PROPERTIES LINKER_LANGUAGE CXX)

TARGET_LINK_LIBRARIES(
        BlazarSamples
        BlazarECS
        BlazarInput
        BlazarScene
        BlazarGraphics
        BlazarPhysics
)