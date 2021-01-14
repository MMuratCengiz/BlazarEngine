#version 450

layout(set = 0, binding = 0) uniform sampler2D litScene;
layout(set = 1, binding = 0) uniform sampler2D skyBoxTex;

layout (location = 0) in vec3 transitPosition;

layout (location = 0) out vec4 outputColor;

void main() {
    outputColor = texture(litScene, transitPosition.xy);

    if ( outputColor.a == 0 )
    {
        outputColor = texture(skyBoxTex, transitPosition.xy);
    }
}