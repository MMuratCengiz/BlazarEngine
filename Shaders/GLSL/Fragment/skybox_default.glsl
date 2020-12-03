#version 450

layout( set = 2, binding = 0 ) uniform samplerCube Texture1;

layout (location = 0) in vec3 transitTextureCoordinates;

layout (location = 0) out vec4 outputColor;

void main() {
    outputColor = texture( Texture1, transitTextureCoordinates );
}
