/*
Copyright (c) 2015-2021 Alternative Games Ltd / Turo Lamminen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#version 450 core

#include "../External/IncludeSMAA.h"
#include "utilities.glsl"

layout (set = 0, binding = 0) uniform Resolution
{
    uint width;
    uint height;
} resolution;

layout (location = 0) out vec2 texcoord;
layout (location = 1) out vec4 offset;

void main(void)
{
    SMAA_RT_METRICS = float4(1.0 / resolution.width, 1.0 / resolution.height, resolution.width, resolution.height);

    vec2 pos_n_texCoord[ 2 ] = getOverSizedTriangle( gl_VertexIndex );
    vec2 pos = pos_n_texCoord[ 0 ];
    texcoord = pos_n_texCoord[ 1 ];

    offset = vec4(0.0, 0.0, 0.0, 0.0);

    SMAANeighborhoodBlendingVS(texcoord, offset);

    gl_Position = vec4(pos, 0.0, 1.0);
}
