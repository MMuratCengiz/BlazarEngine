set(Libs
        external/loaders/stb_image.h
        external/vma/vk_mem_alloc.h
        )


set(Core
        Core/Common.h
        Core/Time.h
        Core/Constants.h
        Core/Utilities.h
        Core/Utilities.cpp
        )


set(Graphics
        Graphics/RenderDevice.h
        Graphics/RenderDevice.cpp
        Graphics/RenderSurface.h
        Graphics/RenderSurface.cpp
        Graphics/Renderer.h
        Graphics/Renderer.cpp
        Graphics/RenderDeviceBuilder.h
        Graphics/RenderDeviceBuilder.cpp
        Graphics/DrawDescription.h
        Graphics/RenderUtilities.h
        Graphics/RenderUtilities.cpp
        Graphics/InstanceContext.h
        Graphics/GraphicsException.h
        Core/Time.cpp
        Graphics/CommonTextures.h
        Graphics/DescriptorManager.cpp
        Graphics/DescriptorManager.h
        Graphics/CommandExecutor.h
        Graphics/CommandExecutor.cpp
        Graphics/CommanExecutorArguments.h
        Graphics/ClientSideLock.h
        Graphics/VmaImplementation.cpp
        Graphics/GLSLShaderSet.h
        Graphics/GLSLShaderSet.cpp
        Graphics/MeshLoader.h
        Graphics/MeshLoader.cpp
        Graphics/MaterialLoader.h
        Graphics/MaterialLoader.cpp
        Graphics/TransformLoader.h
        Graphics/TransformLoader.cpp
        Graphics/CameraLoader.h
        Graphics/CameraLoader.cpp
        Graphics/BuiltinPrimitives.h
        Graphics/PipelineSelector.h
        Graphics/CubeMapLoader.h
        Graphics/CubeMapLoader.cpp
        Samples/Spaceship.h
        Graphics/LightLoader.h
        Graphics/LightLoader.cpp
        Graphics/WorldContextLoader.h
        Graphics/WorldContextLoader.cpp
        )


set(Input
        Input/GlobalEventHandler.cpp
        Input/GlobalEventHandler.h
        Core/DynamicMemory.h
        Input/ActionMap.cpp
        Input/ActionMap.h
        Input/EventHandler.cpp
        Input/EventHandler.h
        )


set(ECS
        ECS/IGameEntity.h
        ECS/Renderable.h
        ECS/IComponent.h
        ECS.h
        ECS/CCollisionObject.h
        ECS/CMesh.h
        ECS/CRigidBody.h
        ECS/ECSUtilities.h
        ECS/CMaterial.h
        ECS/ISystem.h
        ECS/CTransform.h
        ECS/CCubeMap.h
        ECS/CPointLight.h
        ECS/CAmbientLight.h
        ECS/CDirectionalLight.h
        ECS/CSpotLight.h
        )


set(Physics
        Physics/PhysicsWorld.h
        Physics/PhysicsWorld.cpp
        Physics/PhysicsTransformSystem.h
        Physics/PhysicsTransformSystem.cpp
        Physics/CollisionShapeInitializer.h
        Physics/CollisionShapeInitializer.cpp
        )


set(Scene
        Scene/World.h
        Scene/Camera.h
        Scene/FpsCamera.h
        Scene/FpsCamera.cpp
        Scene/Scene.h
        Scene/FPSCounter.h
        Scene/IPlayable.h
        Scene/Window.h
        )


set(Samples
        Samples/SampleCrate.h
        Samples/SampleSetupInputBindings.h
        Samples/SampleSmallCrate.h
        Samples/SampleGame.h
        Samples/SampleGame.cpp
        Samples/SampleHouse.h
        Samples/SampleFloor.h
        Samples/SampleTrafficCone.h
        Samples/SampleCar1.h
        Samples/SampleCar2.h
        Samples/SampleOldHouse.h
        Samples/SampleCubeMap.h
        Samples/Spaceship.h
        )


set(Shaders
        Shaders/GLSL/Fragment/default.glsl
        Shaders/GLSL/Vertex/default.glsl
        )


set(SPVDir
        "${CMAKE_SOURCE_DIR}/external/SPIRV-Cross"
        )


set(SPV
        ${SPVDir}/spirv.hpp
        ${SPVDir}/spirv_cfg.hpp
        ${SPVDir}/spirv_cfg.cpp
        ${SPVDir}/spirv_cpp.hpp
        ${SPVDir}/spirv_cpp.cpp
        ${SPVDir}/spirv_parser.hpp
        ${SPVDir}/spirv_parser.cpp
        ${SPVDir}/spirv_reflect.hpp
        ${SPVDir}/spirv_reflect.cpp
        ${SPVDir}/spirv_cross.hpp
        ${SPVDir}/spirv_cross.cpp
        ${SPVDir}/spirv_cross_util.hpp
        ${SPVDir}/spirv_cross_util.cpp
        ${SPVDir}/spirv_glsl.hpp
        ${SPVDir}/spirv_glsl.cpp
        ${SPVDir}/spirv_cross_parsed_ir.hpp
        ${SPVDir}/spirv_cross_parsed_ir.cpp
        )
