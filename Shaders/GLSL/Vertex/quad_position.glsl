#version 450

layout( location = 0 ) out vec4 outPosition;

void main() {
    outPosition = vec4(vec3(vec2( (gl_VertexIndex << 1 ) & 2, gl_VertexIndex & 2), 0.0f ), 0.0f);
    gl_Position = vec4(transitPosition.xy * 2.0f - 1.0f, 0.0f, 1.0f);
}
