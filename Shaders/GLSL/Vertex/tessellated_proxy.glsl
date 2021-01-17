#version 450

layout(set = 0, binding = 0) uniform ViewProjection {
    mat4 view;
    mat4 proj;
} viewProjection;

layout(push_constant) uniform PushConstants {
    mat4 ModelMatrix;
    mat4 NormalModelMatrix;
} pushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 textureCoor;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTextureCoor;

void main() {
    outPosition = pushConstants.ModelMatrix * vec4(inPosition, 1.0f);
    outNormal = normalize(mat3(pushConstants.NormalModelMatrix) * inNormal);

    outTextureCoor = textureCoor;
    gl_Position = viewProjection.proj * viewProjection.view * outPosition;
}