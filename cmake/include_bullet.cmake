#OPTION(INSTALL_LIBS ON)
#OPTION(BUILD_UNIT_TESTS OFF)
#OPTION(BUILD_OPENGL3_DEMOS OFF)
#OPTION(BUILD_BULLET2_DEMOS OFF)
#SET(PKGCONFIG_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/external/bullet3/libs)

SET(OSDIR "unix")
IF (WIN32)
    SET(OSDIR "win")
ENDIF()

SET(BULLET_BINARY_DIR ${PROJECT_BINARY_DIR}/bullet3)
IF (${BLAZAR_INSTALL_LIBS})
    SET(BULLET_BINARY_DIR ${BLAZAR_INSTALL_LOCATION})
ENDIF()

SET(BULLET_OPTIONS
        -DINSTALL_LIBS=on
        -DPKGCONFIG_INSTALL_PREFIX=${PROJECT_BINARY_DIR}/bullet3/lib
        -DBUILD_BULLET2_DEMOS=OFF
        -DBUILD_CPU_DEMOS=OFF
        -DBUILD_EXTRAS=OFF
        -DBUILD_OPENGL3_DEMOS=OFF
        -DBUILD_UNIT_TESTS=OFF
        -DUSE_DOUBLE_PRECISION:BOOL=OFF
        -DCMAKE_INSTALL_PREFIX:PATH=${BULLET_BINARY_DIR}
        -DINCLUDE_INSTALL_DIR=/include/
        )

ExternalProject_Add(bullet3
        SOURCE_DIR "${PROJECT_SOURCE_DIR}/external/bullet3"
        CMAKE_ARGS ${BULLET_OPTIONS}
        BUILD_ALWAYS 1
        BUILD_IN_SOURCE 1
        PREFIX "bullet3"
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        )

IF (${BLAZAR_INSTALL_LIBS})
    INSTALL(DIRECTORY "${BULLET_BINARY_DIR}/include/bullet/" DESTINATION "${CMAKE_INSTALL_PREFIX}/include")
ENDIF()

INCLUDE_DIRECTORIES("${BULLET_BINARY_DIR}/include/bullet")
LINK_DIRECTORIES("${BULLET_BINARY_DIR}/lib")

LIST(APPEND BlazarSources "${BULLET_BINARY_DIR}/include/bullet")