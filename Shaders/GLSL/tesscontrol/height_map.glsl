#version 450

layout(vertices = 3) out;

layout(location = 0) in vec4 inPosition[];
layout(location = 1) in vec3 inNormal[];
layout(location = 2) in vec2 inTextureCoor[];

layout(location = 0) out vec4 outPosition[3];
layout(location = 1) out vec3 outNormal[3];
layout(location = 2) out vec2 outTextureCoor[3];

layout(set = 4, binding = 0) uniform Tessellation {
    float innerLevel;
    float outerLevel;
} tessellationLevel;

void main() {
    outPosition[gl_InvocationID] = inPosition[gl_InvocationID];
    outNormal[gl_InvocationID]   = inNormal[gl_InvocationID];
    outTextureCoor[gl_InvocationID] = inTextureCoor[gl_InvocationID];
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    if (gl_InvocationID == 0) {
        gl_TessLevelInner[0] = 1000.0f; //tessellationLevel.innerLevel;
        gl_TessLevelInner[1] = 1000.0f; //tessellationLevel.innerLevel;
        gl_TessLevelOuter[0] = 1000.0f; //tessellationLevel.outerLevel;
        gl_TessLevelOuter[1] = 1000.0f; //tessellationLevel.outerLevel;
        gl_TessLevelOuter[2] = 1000.0f; //tessellationLevel.outerLevel;
        gl_TessLevelOuter[3] = 1000.0f; //tessellationLevel.outerLevel;
    }
}