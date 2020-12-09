SET(BlazarPhysicsSources
        BlazarPhysics/PhysicsWorld.h
        BlazarPhysics/PhysicsWorld.cpp
        BlazarPhysics/PhysicsTransformSystem.h
        BlazarPhysics/PhysicsTransformSystem.cpp
        BlazarPhysics/CollisionShapeInitializer.h
        BlazarPhysics/CollisionShapeInitializer.cpp
        )

ADD_LIBRARY(BlazarPhysics ${BlazarPhysicsSources})

SET_TARGET_PROPERTIES(BlazarPhysics PROPERTIES LINKER_LANGUAGE CXX)

TARGET_LINK_LIBRARIES(
        BlazarPhysics
        BlazarCore
        BlazarECS
)