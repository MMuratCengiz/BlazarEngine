FIND_PACKAGE(Vulkan REQUIRED)

SET(BlazarCoreSources
        BlazarCore/DynamicMemory.h
        BlazarCore/Common.h
        BlazarCore/Time.h
        BlazarCore/Constants.h
        BlazarCore/Utilities.h
        BlazarCore/Utilities.cpp
        BlazarCore/Time.cpp
        BlazarCore/SimpleTree.h)

ADD_LIBRARY(BlazarCore ${BlazarCoreSources})

SET_TARGET_PROPERTIES(BlazarCore PROPERTIES LINKER_LANGUAGE CXX)
