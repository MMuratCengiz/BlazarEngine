#version 450

#include "frag_utilities.glsl"

layout( set = 1, binding = 0 ) uniform samplerCube SkyBox;

layout (location = 0) in vec4 transitPosition;

layout (location = 0) out vec4 outputColor;

void main() {
    outputColor = texture( SkyBox, vec3(transitPosition) );
    outputColor = gammaCorrectColor( outputColor );
}
