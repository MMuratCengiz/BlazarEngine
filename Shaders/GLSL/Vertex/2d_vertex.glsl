#version 450 core

layout(set = 0, binding = 0) uniform ViewProjection {
    mat4 view;
    mat4 proj;
} viewProjection;

layout(push_constant) uniform PushConstants {
    mat4 ModelMatrix;
} pushConstants;

layout(set = 1, binding = 0) uniform InstanceData
{
    mat4 model[ 100 ];
    uint instanceCount;
} instanceData;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec2 inTextureCoor;

layout (location = 0) out vec4 outPosition;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec2 outTextureCoor;

void main() {
    mat4 model = pushConstants.ModelMatrix;

    if ( instanceData.instanceCount > 0 && gl_InstanceIndex > 0 )
    {
        model = instanceData.model[ gl_InstanceIndex - 1 ];
    }
    outPosition = model * inPosition;

    gl_Position = viewProjection.proj * viewProjection.view * outPosition;

    outTextureCoor = inTextureCoor;
    outNormal = inNormal;
}
