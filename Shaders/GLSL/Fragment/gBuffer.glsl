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

layout (location = 0) in vec3 transitVertexPosition;
layout (location = 1) in vec3 transitNormal;
layout (location = 2) in vec2 transitTexture1Coor;
layout (location = 3) in vec4 worldPos;

layout (location = 0) out vec4 gBuffer_Position;
layout (location = 1) out vec4 gBuffer_Normal;
layout (location = 2) out vec4 gBuffer_AlbedoSpec;
layout (location = 3) out vec4 gBuffer_Scene;

void main() {
    gBuffer_Position = vec4( transitVertexPosition, 0.0f );
    gBuffer_Normal = vec4( transitNormal, 0.0f );
    gBuffer_AlbedoSpec = vec4( texture( Texture1, transitTexture1Coor * mat.textureScale.xz ).rgb, mat.shininess );
    gBuffer_Scene = worldPos;
}
