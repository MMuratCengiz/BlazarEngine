#version 450

layout (triangles, equal_spacing, cw) in;


layout(set = 2, binding = 0) uniform Material {
    vec4 diffuseColor;
    vec4 specularColor;
    vec4 textureScale;

    float shininess;
    uint hasHeightMap;
} mat;

layout(set = 4, binding = 0) uniform sampler2D HeightMap;

layout(location = 0) in vec4 inPosition[];
layout(location = 1) in vec3 inNormal[];
layout(location = 2) in vec2 inTextureCoor[];

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTextureCoor;

void main() {
    gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) + (gl_TessCoord.y * gl_in[1].gl_Position) + (gl_TessCoord.z * gl_in[2].gl_Position);

    outPosition = gl_TessCoord.x*inPosition[0] + gl_TessCoord.y*inPosition[1] + gl_TessCoord.z*inPosition[2];
    outNormal = gl_TessCoord.x*inNormal[0] + gl_TessCoord.y*inNormal[1] + gl_TessCoord.z*inNormal[2];
    outTextureCoor = gl_TessCoord.x*inTextureCoor[0] + gl_TessCoord.y*inTextureCoor[1] + gl_TessCoord.z*inTextureCoor[2];

    if (mat.hasHeightMap == 1)
    {
//        gl_Position.y += texture(HeightMap, outTextureCoor * mat.textureScale.xz).r * 10.0f;
    }
}
