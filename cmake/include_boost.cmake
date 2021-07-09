CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

SET(CORE_BOOST_LIBS
        algorithm
        predef
        system
        ratio
        callable_traits
        move
        io
        functional
        bind
        integer
        tuple
        optional
        asio
        type_index
        detail
        container_hash
        chrono
        numeric/conversion
        core
        locale
        fusion
        array
        proto
        type_traits
        static_assert
        utility
        iterator
        function
        mpl
        phoenix
        typeof
        intrusive
        lexical_cast
        container
        log
        mp11
        preprocessor
        parameter
        smart_ptr
        assert
        exception
        throw_exception
        date_time
        random
        generator_iterator
        range
        math
        concept_check
        system
        filesystem
        config)

FOREACH (CORE_BOOST_LIB IN LISTS CORE_BOOST_LIBS)
    TARGET_INCLUDE_DIRECTORIES(${INCLUDE_BOOST_TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/external/boost/libs/${CORE_BOOST_LIB}/include)
ENDFOREACH ()

SET(PLATFORM_BOOST_LIBS winapi)

FOREACH (PLATFORM_BOOST_LIB IN LISTS PLATFORM_BOOST_LIBS)
    TARGET_INCLUDE_DIRECTORIES(${INCLUDE_BOOST_TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/external/boost/libs/${PLATFORM_BOOST_LIB}/include)
ENDFOREACH ()

FOREACH (REQUESTED_BOOST_LIB IN LISTS REQUESTED_BOOST_LIBS)
    TARGET_INCLUDE_DIRECTORIES(${INCLUDE_BOOST_TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/external/boost/libs/${REQUESTED_BOOST_LIB}/include)
ENDFOREACH ()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_DEBUG_LIBS ON)
SET(BOOST_ROOT ${CMAKE_SOURCE_DIR}/external/boost/)
SET(Boost_DIR ${CMAKE_SOURCE_DIR}/external/boost/ CACHE INTERNAL "Boost Directory")
SET(BOOST_INCLUDEDIR ${CMAKE_SOURCE_DIR}/external/boost/libs)
SET(BOOST_LIBRARYDIR ${CMAKE_SOURCE_DIR}/external/boost/stage/lib)

find_package(Boost REQUIRED)

message(STATUS ${Boost_FOUND})
message(STATUS ${Boost_LIBRARIES})

TARGET_LINK_DIRECTORIES(${INCLUDE_BOOST_TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/external/boost/stage/lib)
TARGET_LINK_LIBRARIES(${INCLUDE_BOOST_TARGET} ws2_32 ${Boost_LIBRARIES}#[[libboost_thread libboost_system libboost_filesystem]])