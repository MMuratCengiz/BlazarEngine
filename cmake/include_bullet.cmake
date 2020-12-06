set(INSTALL_LIBS ON)

add_subdirectory("${CMAKE_SOURCE_DIR}/external/bullet3")

include_directories(
        "${CMAKE_SOURCE_DIR}/external/bullet3/src"
)
#
#link_directories(BlazarEngine
#        "${CMAKE_SOURCE_DIR}/external/bullet3/libs")


include( "${CMAKE_BINARY_DIR}/external/bullet3/BulletConfig.cmake" )
include( "${CMAKE_SOURCE_DIR}/external/bullet3/UseBullet.cmake" )