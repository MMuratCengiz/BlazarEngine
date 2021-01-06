#version 450

layout(set = 0, binding = 0) uniform sampler2D defaultPass_Result;
layout(set = 1, binding = 0) uniform sampler2D defaultPass_ResultRed;

layout (location = 0) in vec3 transitPosition;

layout (location = 0) out vec4 outputColor;

void main() {
    vec4 t1 = texture( defaultPass_Result, transitPosition.xy );
    vec4 t2 = texture( defaultPass_ResultRed, transitPosition.xy );

    outputColor = t1 * t2;
}
