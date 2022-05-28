INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)

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

IF (BLAZAR_INSTALL_LIBS)
    INSTALL_TARGET(BlazarECS)
    INSTALL(FILES ${BlazarECSHeaders} DESTINATION include/BlazarECS)
ENDIF()

SET_TARGET_PROPERTIES(BlazarECS PROPERTIES LINKER_LANGUAGE CXX)

SET(INCLUDE_BOOST_TARGET BlazarECS)
SET(REQUESTED_BOOST_LIBS container)

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/include_boost.cmake)

SET(ECS_LINK_LIBRARIES BlazarCore PARENT_SCOPE)
TARGET_INCLUDE_DIRECTORIES(BlazarECS PUBLIC ${PROJECT_SOURCE_DIR}/Core)