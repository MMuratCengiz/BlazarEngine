#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 2, binding = 0) uniform Material {
    vec4 diffuseColor;
    vec4 specularColor;
    vec4 textureScale;

    float shininess;
    uint hasHeightMap;
} mat;

layout(set = 3, binding = 0) uniform sampler2D Texture1;

layout(set = 5, binding = 0) uniform OutlineColor {
    vec4 rgba;
} outlineColor;

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec4 inNormal;
layout (location = 2) in vec2 inTextureCoor;

layout (location = 0) out vec4 gBuffer_Position;
layout (location = 1) out vec4 gBuffer_Normal;
layout (location = 2) out vec4 gBuffer_Albedo;
layout (location = 3) out vec4 gBuffer_Material;

void main() {
    gBuffer_Position = inPosition;
    gBuffer_Normal = inNormal;
    gBuffer_Albedo = outlineColor.rgba;
    gBuffer_Material = vec4( mat.shininess );
}
