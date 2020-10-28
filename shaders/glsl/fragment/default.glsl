#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec4 outputColor;
layout (location = 0) in vec3 fragColor;

void main() {
    outputColor = vec4(fragColor, 1.0f);
}
