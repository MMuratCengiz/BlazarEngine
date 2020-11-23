#version 450

layout(binding = 0) uniform SceneMVP {
    mat4 view;
    mat4 proj;
} mvp;

layout(binding = 1) uniform model {
    mat4 element;
} m;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 texture1Coor;

layout (location = 0) out vec2 transitTexture1Coor;

void main() {
    gl_Position = mvp.proj * mvp.view * m.element * vec4(inPosition, 1.0f);

    transitTexture1Coor = texture1Coor;
}
