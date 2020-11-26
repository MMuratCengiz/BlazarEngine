#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform sampler2D Texture1;

layout (location = 0) in vec2 transitTexture1Coor;

layout (location = 0) out vec4 outputColor;

void main() {
    outputColor = texture(Texture1, transitTexture1Coor);
}
