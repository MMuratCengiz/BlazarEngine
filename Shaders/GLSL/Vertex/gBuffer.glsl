#version 450

layout(set = 0, binding = 0) uniform ViewProjection {
    mat4 view;
    mat4 proj;
} vp;

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
    vec4 worldPos4 = pushConstants.ModelMatrix * vec4(inPosition, 1.0f);
    vec4 vpPosition = vp.proj * vp.view * worldPos4;

    transitVertexPosition = vpPosition.xyz;
    gl_Position = vpPosition;

    transitWorldPos = worldPos4;

    transitTexture1Coor = texture1Coor;

    transitNormal = normalize(mat3(pushConstants.NormalModelMatrix) * inNormal);
}
