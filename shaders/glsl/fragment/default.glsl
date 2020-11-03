#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texture1;

layout (location = 0) in vec2 transitTexture1Coor;

layout (location = 0) out vec4 outputColor;

void main() {
    outputColor = texture( texture1, transitTexture1Coor );
}
