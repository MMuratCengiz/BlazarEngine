#OPTION(INSTALL_LIBS ON)
#OPTION(BUILD_UNIT_TESTS OFF)
#OPTION(BUILD_OPENGL3_DEMOS OFF)
#OPTION(BUILD_BULLET2_DEMOS OFF)
#SET(PKGCONFIG_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/external/bullet3/libs)

SET(OSDIR "unix")
IF (WIN32)
    SET(OSDIR "win")
ENDIF()

SET(BULLET_OPTIONS
        -DINSTALL_LIBS=on
        -DPKGCONFIG_INSTALL_PREFIX=${PROJECT_BINARY_DIR}/bullet3/lib/pkgconfig
        -DBUILD_BULLET2_DEMOS=off
        -DBUILD_CPU_DEMOS=off
        -DBUILD_OPENGL3_DEMOS=off
        -DBUILD_UNIT_TESTS=off
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX:PATH=${PROJECT_BINARY_DIR}/bullet3
        )

ExternalProject_Add(bullet3
        SOURCE_DIR "${PROJECT_SOURCE_DIR}/external/bullet3"
        CMAKE_ARGS ${BULLET_OPTIONS}
        BUILD_ALWAYS 1
        BUILD_IN_SOURCE 1
        PREFIX "bullet3"
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        )

INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}/bullet3/include/bullet")
LINK_DIRECTORIES("${PROJECT_BINARY_DIR}/bullet3/lib")
LINK_LIBRARIES(
        libBulletSoftBody.a
        libBulletDynamics.a
        libBullet3Dynamics.a
        libBullet3Geometry.a
        libBulletCollision.a
        libBullet3Collision.a
        libBullet3Common.a
        libBulletInverseDynamics.a
        libLinearMath.a
)

LIST(APPEND BlazarSources "${PROJECT_SOURCE_DIR}/external/bullet3/include")