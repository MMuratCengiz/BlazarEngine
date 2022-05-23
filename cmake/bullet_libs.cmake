SET(POSTFIX "")

IF(MSVC)
    SET(POSTFIX "_${CMAKE_BUILD_TYPE}")
ENDIF()

SET(BULLET_LIBS
        Bullet3Dynamics${POSTFIX}
        BulletDynamics${POSTFIX}
        BulletSoftBody${POSTFIX}
        Bullet3Geometry${POSTFIX}
        Bullet3Collision${POSTFIX}
        BulletCollision${POSTFIX}
        BulletInverseDynamics${POSTFIX}
        LinearMath${POSTFIX}
        Bullet3Common${POSTFIX}
        )