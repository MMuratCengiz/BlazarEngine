SET(BlazarECSSources
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
        BlazarECS/ComponentTable.cpp
        )

ADD_LIBRARY(BlazarECS ${BlazarECSSources})

SET_TARGET_PROPERTIES(BlazarECS PROPERTIES LINKER_LANGUAGE CXX)

TARGET_LINK_LIBRARIES(BlazarECS
        BlazarCore
        )