#version 450

layout( set = 1, binding = 0 ) uniform samplerCube SkyBox;

layout (location = 0) in vec3 transitPosition;

layout (location = 0) out vec4 outputColor;

void main() {
    outputColor = texture( SkyBox, transitPosition );
}
