FIND_PACKAGE(Vulkan REQUIRED)

SET(BlazarCoreSources
        BlazarCore/DynamicMemory.h
        BlazarCore/Common.h
        BlazarCore/Time.h
        BlazarCore/Constants.h
        BlazarCore/Utilities.h
        BlazarCore/Utilities.cpp
        BlazarCore/Time.cpp
        BlazarCore/SimpleTree.h 
        BlazarCore/Logger.cpp
        BlazarCore/Logger.h)

ADD_LIBRARY(BlazarCore ${BlazarCoreSources})

SET(INCLUDE_BOOST_TARGET BlazarCore)
SET(REQUESTED_BOOST_LIBS thread format atomic log)

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/include_boost.cmake)

SET_TARGET_PROPERTIES(BlazarCore PROPERTIES LINKER_LANGUAGE CXX)
