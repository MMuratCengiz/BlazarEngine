#version 450

layout( set = 1, binding = 0 ) uniform samplerCube SkyBox;

layout (location = 0) in vec3 transitTextureCoordinates;

layout (location = 0) out vec4 outputColor;
layout (location = 1) out vec4 outputColorRed;

void main() {
    outputColor = texture( SkyBox, transitTextureCoordinates );
    outputColorRed = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
}
