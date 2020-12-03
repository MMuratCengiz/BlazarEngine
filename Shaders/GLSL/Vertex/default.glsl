#version 450

layout(set = 0, binding = 0) uniform ViewProjection {
    mat4 view;
    mat4 proj;
} vp;

layout(set = 1, binding = 0) uniform WorldContext {
    vec4 worldPosition;
} wContext;

layout(push_constant) uniform Model {
    mat4 element;
} m;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 texture1Coor;

layout (location = 0) out vec2 transitTexture1Coor;
layout (location = 1) out vec3 transitNormal;
layout (location = 2) out vec3 transitWorldPos;

void main() {
    gl_Position = vp.proj * vp.view * m.element * vec4(inPosition, 1.0f);

    transitTexture1Coor = texture1Coor;
    transitNormal = normalize(inNormal);
    transitWorldPos = wContext.worldPosition.xyz;
}
