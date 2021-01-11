#version 450

layout(set = 1, binding = 0) uniform sampler2D Texture1;
layout(set = 2, binding = 0) uniform Material {
    vec4 diffuseColor;
    vec4 specularColor;
    vec4 textureScale;

    float shininess;
} mat;


layout(location = 0) in vec2 transitTextureCoordinates;

layout(location = 0) out vec4 shadowMap;

void main() {
    shadowMap = vec4( gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0f ); //texture( Texture1, transitTextureCoordinates * mat.textureScale.xz );
}
