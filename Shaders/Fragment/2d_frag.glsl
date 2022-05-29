#version 450

layout(set = 2, binding = 0) uniform sampler2D Texture1;

layout (location = 0) in vec4 outPosition;
layout (location = 1) in vec2 outTextureCoor;

layout (location = 0) out vec4 outputColor;

void main() {
    outputColor = texture(Texture1, outTextureCoor.xy);
}
