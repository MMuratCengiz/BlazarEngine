#version 450

layout(set = 0, binding = 0) uniform ViewProjection {
    mat4 view;
    mat4 proj;
} vp;

layout(set = 1, binding = 0) uniform sampler2D HeightMap;

layout(set = 2, binding = 0) uniform Material {
    vec4 diffuseColor;
    vec4 specularColor;
    vec4 textureScale;

    float shininess;
    uint hasHeightMap;
} mat;

layout(push_constant) uniform PushConstants {
    mat4 ModelMatrix;
    mat4 NormalModelMatrix;
} pushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 texture1Coor;

layout (location = 0) out vec3 transitVertexPosition;
layout (location = 1) out vec3 transitNormal;
layout (location = 2) out vec2 transitTexture1Coor;
layout (location = 3) out vec4 transitWorldPos;

void main() {
    vec3 positionWithHeightMap = inPosition;

    vec4 worldPos4 = pushConstants.ModelMatrix * vec4(inPosition, 1.0f);

    if ( mat.hasHeightMap == 1 )
    {
        worldPos4.y = length( texture( HeightMap, texture1Coor * mat.textureScale.xz ).r );
    }

    vec4 vpPosition = vp.proj * vp.view * worldPos4;

    transitVertexPosition = vpPosition.xyz;
    gl_Position = vpPosition;

    transitWorldPos = worldPos4;

    transitTexture1Coor = texture1Coor;

    transitNormal = normalize(mat3(pushConstants.NormalModelMatrix) * inNormal);
}
