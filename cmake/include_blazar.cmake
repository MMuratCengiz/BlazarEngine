SET(Libs
        external/loaders/stb_image.h
        external/vma/vk_mem_alloc.h
        )

SET(Shaders
        Shaders/GLSL/Fragment/default.glsl
        Shaders/GLSL/Vertex/default.glsl
        )


SET(SPVDir
        "${CMAKE_SOURCE_DIR}/external/SPIRV-Cross"
        )


SET(SPV
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

ADD_SUBDIRECTORY("${CMAKE_SOURCE_DIR}/3DModelLoader/")

INCLUDE_DIRECTORIES(
        "${CMAKE_SOURCE_DIR}/3DModelLoader/src"
)