INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)

FIND_PACKAGE(Vulkan REQUIRED)
FIND_PACKAGE(glfw3 CONFIG REQUIRED)
FIND_PACKAGE(glm REQUIRED)
FIND_PACKAGE(Boost COMPONENTS thread atomic log REQUIRED)
FIND_PACKAGE(Bullet CONFIG REQUIRED)

FIND_PATH(STB_INCLUDE_DIRS "stb_image.h")

SET(BlazarCoreSources
        src/BlazarCore/Utilities.cpp
        src/BlazarCore/Time.cpp
        src/BlazarCore/Logger.cpp)

ADD_LIBRARY(BlazarCore ${BLAZAR_LIB_TYPE} ${BlazarCoreSources})

TARGET_INCLUDE_DEFAULT_DIRECTORIES(BlazarCore)
TARGET_LINK_LIBRARIES(BlazarCore
        PUBLIC
            Boost::boost
            Boost::thread
            Boost::atomic
            Boost::log
            glm::glm
            glfw
            ${Vulkan_LIBRARY}
            ${BULLET_LIBRARIES}
)
TARGET_INCLUDE_DIRECTORIES(BlazarCore PUBLIC ${STB_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIR})
INSTALL_TARGET(BlazarCore)

SET_TARGET_PROPERTIES(BlazarCore PROPERTIES LINKER_LANGUAGE CXX)