#version 450 core

layout(set = 0, binding = 0) uniform ViewProjection {
    mat4 view;
    mat4 proj;
} viewProjection;

layout(push_constant) uniform PushConstants {
    mat4 ModelMatrix;
    mat4 NormalModelMatrix;
} pushConstants;

layout(set = 1, binding = 0) uniform InstanceData
{
    mat4 model[ 100 ];
    uint instanceCount;
} instanceData;

layout(set = 4, binding = 0) uniform BoneTransformations
{
    mat4 boneTransformation[ 100 ];
    uint transformationCount;
} instanceData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoor;
layout(location = 3) in vec4 boneIds;
layout(location = 4) in vec4 boneWeights;

layout (location = 0) out vec4 outPosition;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec2 outTextureCoor;

void main() {
    mat4 model = pushConstants.ModelMatrix;

    if ( instanceData.instanceCount > 0 && gl_InstanceIndex > 0 )
    {
        model = instanceData.model[ gl_InstanceIndex - 1 ];
    }

    outPosition = model * vec4(inPosition, 1.0f);

    gl_Position = viewProjection.proj * viewProjection.view * outPosition;

    outTextureCoor = inTextureCoor;
    outNormal = normalize(mat3(pushConstants.NormalModelMatrix) * inNormal);
}
