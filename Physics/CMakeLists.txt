INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/bullet_libs.cmake)

SET(BlazarPhysicsHeaders
        BlazarPhysics/PhysicsWorld.h
        BlazarPhysics/PhysicsTransformSystem.h
        BlazarPhysics/CollisionShapeInitializer.h
        )

SET(BlazarPhysicsSources
        BlazarPhysics/PhysicsWorld.cpp
        BlazarPhysics/PhysicsTransformSystem.cpp
        BlazarPhysics/CollisionShapeInitializer.cpp
        )

ADD_LIBRARY(BlazarPhysics ${BLAZAR_LIB_TYPE} ${BlazarPhysicsHeaders} ${BlazarPhysicsSources})

IF (${BLAZAR_INSTALL_LIBS})
    INSTALL_TARGET(BlazarPhysics)
    INSTALL(FILES ${BlazarPhysicsHeaders} DESTINATION include/BlazarPhysics)
ENDIF()

SET_TARGET_PROPERTIES(BlazarPhysics PROPERTIES LINKER_LANGUAGE CXX)

TARGET_LINK_LIBRARIES(
        BlazarPhysics
        BlazarCore
        BlazarECS
        ${BULLET_LIBS}
)