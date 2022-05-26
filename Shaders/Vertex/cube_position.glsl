#version 450

layout(set = 0, binding = 0) uniform ViewProjection {
    mat4 view;
    mat4 proj;
} vp;

layout( location = 0 ) in vec4 position;

layout (location = 0) out vec4 transitPosition;

void main() {
    transitPosition = position;
    vec4 pos = vp.proj * mat4(mat3(vp.view)) * position;
    gl_Position = pos.xyww;
}
