#version 450

layout(set = 0, binding = 0) uniform sampler2D aliasedImage;
layout(set = 1, binding = 0) uniform sampler2D skyBoxTex;

layout (location = 0) in vec3 inPosition;

layout (location = 0) out vec4 outputColor;

void main() {
    outputColor = texture(aliasedImage, inPosition.xy);

    if (outputColor.a == 0)
    {
        outputColor = texture(skyBoxTex, inPosition.xy);
    }
}