SET(GLFW_LIBS
        ${BLAZAR_INSTALL_LOCATION}lib/libglfw3.a
        )

IF(APPLE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo")
ENDIF()