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
        range
        math
        concept_check
        system
        filesystem
        config)

FOREACH (CORE_BOOST_LIB IN LISTS CORE_BOOST_LIBS)
    TARGET_INCLUDE_DIRECTORIES(${INCLUDE_BOOST_TARGET} PUBLIC ${PROJECT_SOURCE_DIR}/external/boost/libs/${CORE_BOOST_LIB}/include)
    IF (${BLAZAR_INSTALL_LIBS})
        INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/external/boost/libs/${CORE_BOOST_LIB}/include/ DESTINATION include)
    ENDIF()
ENDFOREACH ()

SET(PLATFORM_BOOST_LIBS winapi)

FOREACH (PLATFORM_BOOST_LIB IN LISTS PLATFORM_BOOST_LIBS)
    TARGET_INCLUDE_DIRECTORIES(${INCLUDE_BOOST_TARGET} PUBLIC ${PROJECT_SOURCE_DIR}/external/boost/libs/${PLATFORM_BOOST_LIB}/include)
    IF (${BLAZAR_INSTALL_LIBS})
        INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/external/boost/libs/${PLATFORM_BOOST_LIB}/include/ DESTINATION include)
    ENDIF()
ENDFOREACH ()

FOREACH (REQUESTED_BOOST_LIB IN LISTS REQUESTED_BOOST_LIBS)
    TARGET_INCLUDE_DIRECTORIES(${INCLUDE_BOOST_TARGET} PUBLIC ${PROJECT_SOURCE_DIR}/external/boost/libs/${REQUESTED_BOOST_LIB}/include)
    IF (${BLAZAR_INSTALL_LIBS})
        INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/external/boost/libs/${REQUESTED_BOOST_LIB}/include/ DESTINATION include)
    ENDIF()
ENDFOREACH ()