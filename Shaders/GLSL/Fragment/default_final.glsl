#version 450

layout(set = 0, binding = 0) uniform sampler2D defaultPass_Result;

layout (location = 0) in vec3 transitPosition;

layout (location = 0) out vec4 outputColor;

void main() {
    outputColor = texture( defaultPass_Result, transitPosition.xy );
}
