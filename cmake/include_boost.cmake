SET(CORE_BOOST_LIBS
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
        type_index
        detail
        container_hash
        chrono
        numeric/conversion
        core
        type_traits
        static_assert
        utility
        iterator
        function
        mpl
        preprocessor
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
        config)

FOREACH (CORE_BOOST_LIB IN LISTS CORE_BOOST_LIBS)
    TARGET_INCLUDE_DIRECTORIES(${INCLUDE_BOOST_TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/external/boost/libs/${CORE_BOOST_LIB}/include)
ENDFOREACH ()

SET(PLATFORM_BOOST_LIBS winapi )

FOREACH (PLATFORM_BOOST_LIB IN LISTS PLATFORM_BOOST_LIBS)
    TARGET_INCLUDE_DIRECTORIES(${INCLUDE_BOOST_TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/external/boost/libs/${PLATFORM_BOOST_LIB}/include)
ENDFOREACH ()

FOREACH (REQUESTED_BOOST_LIB IN LISTS REQUESTED_BOOST_LIBS)
    TARGET_INCLUDE_DIRECTORIES(${INCLUDE_BOOST_TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/external/boost/libs/${REQUESTED_BOOST_LIB}/include)
ENDFOREACH ()