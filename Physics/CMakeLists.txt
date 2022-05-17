INCLUDE(${CMAKE_SOURCE_DIR}/cmake/include_definitions.cmake)

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

IF (DEFINED BLAZAR_INSTALL_LIBS)
    INSTALL(TARGETS BlazarPhysics DESTINATION ${BLAZAR_INSTALL_LOCATION}lib)
    INSTALL(FILES ${BlazarPhysicsHeaders} DESTINATION ${BLAZAR_INSTALL_LOCATION}include)
ENDIF()

SET_TARGET_PROPERTIES(BlazarPhysics PROPERTIES LINKER_LANGUAGE CXX)

TARGET_LINK_LIBRARIES(
        BlazarPhysics
        BlazarCore
        BlazarECS
        LinearMath
        Bullet3Common
        Bullet3Geometry
        BulletSoftBody
        BulletDynamics
        Bullet3Dynamics
        Bullet3Collision
        BulletCollision
)