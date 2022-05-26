#version 450

#define MAX_ALLOWED_SHADOW_CASTERS 3

layout(push_constant) uniform PushConstants {
    mat4 ModelMatrix;
} pushConstants;

layout(set = 0, binding = 0) uniform LightViewProjectionMatrix {
    mat4[ MAX_ALLOWED_SHADOW_CASTERS ] casters;
    int arraySize;
} lvpm;

layout(set = 1, binding = 0) uniform InstanceData
{
    mat4 model[ 100 ];
    uint instanceCount;
} instanceData;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec2 inTextureCoor;

void main() {
    mat4 model = pushConstants.ModelMatrix;

    if ( instanceData.instanceCount > 0 && gl_InstanceIndex > 0 )
    {
        model = instanceData.model[ gl_InstanceIndex - 1 ];
    }

    gl_Position = lvpm.casters[ 0 ] * model * inPosition;
}
