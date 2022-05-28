INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)

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

IF (BLAZAR_INSTALL_LIBS)
    INSTALL_TARGET(BlazarPhysics)
    INSTALL(FILES ${BlazarPhysicsHeaders} DESTINATION include/BlazarPhysics)
ENDIF()

SET_TARGET_PROPERTIES(BlazarPhysics PROPERTIES LINKER_LANGUAGE CXX)

TARGET_INCLUDE_DIRECTORIES(BlazarPhysics PUBLIC
        ${PROJECT_SOURCE_DIR}/Core
        ${PROJECT_SOURCE_DIR}/ECS
        )

SET(Physics_LINK_LIBRARIES BlazarCore BlazarECS PARENT_SCOPE)