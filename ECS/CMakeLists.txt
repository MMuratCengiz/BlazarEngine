SET(BlazarECSHeaders
        BlazarECS/ECS.h
        BlazarECS/IGameEntity.h
        BlazarECS/IComponent.h
        BlazarECS/CCollisionObject.h
        BlazarECS/CMesh.h
        BlazarECS/CRigidBody.h
        BlazarECS/ECSUtilities.h
        BlazarECS/CMaterial.h
        BlazarECS/ISystem.h
        BlazarECS/CTransform.h
        BlazarECS/CCubeMap.h
        BlazarECS/CPointLight.h
        BlazarECS/CAmbientLight.h
        BlazarECS/CDirectionalLight.h
        BlazarECS/CSpotLight.h
        BlazarECS/CCamera.h
        BlazarECS/ComponentTable.h
        BlazarECS/CTessellation.h
        BlazarECS/CInstances.h
        BlazarECS/CGameState.h
        BlazarECS/COutlined.h
        BlazarECS/CAnimState.h)

SET(BlazarECSSources
        BlazarECS/ComponentTable.cpp)

ADD_LIBRARY(BlazarECS ${BLAZAR_LIB_TYPE} ${BlazarECSHeaders} ${BlazarECSSources})

IF (${BLAZAR_INSTALL_LIBS})
    INSTALL(TARGETS BlazarECS DESTINATION ${BLAZAR_INSTALL_LOCATION}lib)
    INSTALL(FILES ${BlazarECSHeaders} DESTINATION ${BLAZAR_INSTALL_LOCATION}include/BlazarECS)
ENDIF()

SET_TARGET_PROPERTIES(BlazarECS PROPERTIES LINKER_LANGUAGE CXX)

SET(INCLUDE_BOOST_TARGET BlazarECS)
SET(REQUESTED_BOOST_LIBS container)

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/include_boost.cmake)

TARGET_LINK_LIBRARIES(BlazarECS
        BlazarCore
        )