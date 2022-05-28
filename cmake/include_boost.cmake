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

SET(MERGED_BOOST_LIBS ${CORE_BOOST_LIBS} ${PLATFORM_BOOST_LIBS} ${REQUESTED_BOOST_LIBS})
FOREACH (BOOST_LIB IN LISTS MERGED_BOOST_LIBS)
    SET(BOOST_LIB_INCLUDE ${PROJECT_SOURCE_DIR}/external/boost/libs/${BOOST_LIB}/include)
    TARGET_INCLUDE_DIRECTORIES(${INCLUDE_BOOST_TARGET} PUBLIC ${BOOST_LIB_INCLUDE})
    LIST(APPEND BOOST_INCLUDE_DIRS ${BOOST_LIB_INCLUDE})

    IF (BLAZAR_INSTALL_LIB)
        INSTALL(DIRECTORY ${BOOST_LIB_INCLUDE})
    ENDIF()
ENDFOREACH ()

STRING(REPLACE "Blazar" "" STRIPPED_BOOST_TARGET ${INCLUDE_BOOST_TARGET})
SET(${STRIPPED_BOOST_TARGET}_INCLUDE_DIRS ${BOOST_INCLUDE_DIRS} PARENT_SCOPE)
