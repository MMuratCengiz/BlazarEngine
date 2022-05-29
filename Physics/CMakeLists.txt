INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)

SET(BlazarPhysicsSources
        src/BlazarPhysics/PhysicsWorld.cpp
        src/BlazarPhysics/PhysicsTransformSystem.cpp
        src/BlazarPhysics/CollisionShapeInitializer.cpp
        )

ADD_LIBRARY(BlazarPhysics ${BLAZAR_LIB_TYPE} ${BlazarPhysicsSources})

TARGET_INCLUDE_DEFAULT_DIRECTORIES(BlazarPhysics)
TARGET_LINK_LIBRARIES(BlazarPhysics
        PUBLIC
            BlazarCore
            BlazarECS
        )

INSTALL_TARGET(BlazarPhysics)
SET_TARGET_PROPERTIES(BlazarPhysics PROPERTIES LINKER_LANGUAGE CXX)