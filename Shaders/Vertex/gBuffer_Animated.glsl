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
    mat4 model[100];
    uint instanceCount;
} instanceData;

layout(set = 4, binding = 0) uniform BoneTransformations
{
    mat4 data[100];
    uint size;
} boneTransformations;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec2 inTextureCoor;
layout(location = 3) in vec4 boneIds;
layout(location = 4) in vec4 boneWeights;

layout (location = 0) out vec4 outPosition;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec2 outTextureCoor;

void main() {
    mat4 model = pushConstants.ModelMatrix;

    if (instanceData.instanceCount > 0 && gl_InstanceIndex > 0)
    {
        model = instanceData.model[gl_InstanceIndex - 1];
    }

    mat4 jointMatrix = mat4(1.0);

    if (boneTransformations.size > 0)
    {
        jointMatrix =
            (boneWeights.x * boneTransformations.data[int(boneIds.x)]) +
            (boneWeights.y * boneTransformations.data[int(boneIds.y)]) +
            (boneWeights.z * boneTransformations.data[int(boneIds.z)]) +
            (boneWeights.w * boneTransformations.data[int(boneIds.w)]);

        if ( jointMatrix == mat4(0) )
        {
            jointMatrix = mat4(1);
        }
    }

    outPosition = model * jointMatrix * inPosition;

    gl_Position = viewProjection.proj * viewProjection.view * outPosition;

    outTextureCoor = inTextureCoor;
    outNormal = vec4(normalize(mat3(pushConstants.NormalModelMatrix) * vec3(inNormal)), 0.0f);
}
