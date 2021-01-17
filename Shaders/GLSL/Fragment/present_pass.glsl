#version 450

const float gammaCorrection = 2.2;

layout(set = 0, binding = 0) uniform sampler2D litScene;
layout(set = 1, binding = 0) uniform sampler2D skyBoxTex;

layout (location = 0) in vec3 inPosition;

layout (location = 0) out vec4 outputColor;

void main() {
    outputColor = texture(litScene, inPosition.xy);

    if (outputColor.a == 0)
    {
        outputColor = texture(skyBoxTex, inPosition.xy);
    }

    outputColor = vec4(pow(outputColor.rgb, vec3(1 / gammaCorrection)), outputColor.a);
}