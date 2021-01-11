#version 450

#define MAX_ALLOWED_SHADOW_CASTERS 3

layout(push_constant) uniform PushConstants {
    mat4 ModelMatrix;
} pushConstants;

layout(set = 0, binding = 0) uniform LightViewProjectionMatrix {
    mat4[ MAX_ALLOWED_SHADOW_CASTERS ] casters;
    int arraySize;
} lvpm;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 texture1Coor;

layout(location = 0) out vec2 transitTextureCoordinates;

void main() {
    transitTextureCoordinates = texture1Coor;
    gl_Position = lvpm.casters[ 0 ] * pushConstants.ModelMatrix * vec4( inPosition, 1.0f );
}
