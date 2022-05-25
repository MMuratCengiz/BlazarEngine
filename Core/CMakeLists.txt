INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)

FIND_PACKAGE(Vulkan REQUIRED)

SET(BlazarCoreHeaders
        BlazarCore/DynamicMemory.h
        BlazarCore/Common.h
        BlazarCore/Time.h
        BlazarCore/Constants.h
        BlazarCore/Utilities.h
        BlazarCore/SimpleTree.h
        BlazarCore/Logger.h)

SET(BlazarCoreSources
        BlazarCore/Utilities.cpp
        BlazarCore/Time.cpp
        BlazarCore/Logger.cpp)

ADD_LIBRARY(BlazarCore ${BLAZAR_LIB_TYPE} ${BlazarCoreHeaders} ${BlazarCoreSources})

IF (BLAZAR_INSTALL_LIBS)
    INSTALL_TARGET(BlazarCore)
    INSTALL(FILES ${BlazarCoreHeaders} DESTINATION include/BlazarCore)
ENDIF()

SET(INCLUDE_BOOST_TARGET BlazarCore)
SET(REQUESTED_BOOST_LIBS format atomic log)

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/include_boost.cmake)

SET_TARGET_PROPERTIES(BlazarCore PROPERTIES LINKER_LANGUAGE CXX)
