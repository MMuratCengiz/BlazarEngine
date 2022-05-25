SET(GLFW_LIBS glfw3)

IF(APPLE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo")
ENDIF()