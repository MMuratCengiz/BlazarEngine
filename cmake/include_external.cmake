SET(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external/stb")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external/miniz")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external/vma")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external/tinygltf")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/external/glm")

LIST(APPEND BlazarSources "${PROJECT_SOURCE_DIR}/external/stb")
LIST(APPEND BlazarSources "${PROJECT_SOURCE_DIR}/external/miniz")
LIST(APPEND BlazarSources "${PROJECT_SOURCE_DIR}/external/vma")
LIST(APPEND BlazarSources "${PROJECT_SOURCE_DIR}/external/tinygltf")

ADD_SUBDIRECTORY("${PROJECT_SOURCE_DIR}/external/glm")
ADD_SUBDIRECTORY("${PROJECT_SOURCE_DIR}/external/glfw")

# IMPORT LUA #################################################
ExternalProject_Add(lua
        URL "${PROJECT_SOURCE_DIR}/external/lua-5.4.4.tar.gz"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND make generic
        BUILD_ALWAYS true
        BUILD_IN_SOURCE true'
        INSTALL_COMMAND ""
        PREFIX "lua"
        )

ExternalProject_Get_property(lua SOURCE_DIR)
INCLUDE_DIRECTORIES(${SOURCE_DIR}/src)
SET(LUA_LIB ${SOURCE_DIR}/src/liblua.a)
###############################################################

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/external/sol3)