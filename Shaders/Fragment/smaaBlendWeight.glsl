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

#define SHADER_STAGE_FRAGMENT
#include "../External/IncludeSMAA.h"

layout (set = 0, binding = 0) uniform Resolution
{
    uint width;
    uint height;
} resolution;

layout(set = 1, binding = 0) uniform sampler2D edgesTex;
layout(set = 2, binding = 0) uniform sampler2D areaTex;
layout(set = 3, binding = 0) uniform sampler2D searchTex;

layout (location = 0) in vec2 texcoord;
layout (location = 1) in vec2 pixcoord;
layout (location = 2) in vec4 offset0;
layout (location = 3) in vec4 offset1;
layout (location = 4) in vec4 offset2;

layout (location = 0) out vec4 blendTex;

void main(void)
{
    SMAA_RT_METRICS = float4(1.0 / resolution.width, 1.0 / resolution.height, resolution.width, resolution.height);

    vec4 offsets[3];
    offsets[0] = offset0;
    offsets[1] = offset1;
    offsets[2] = offset2;

    blendTex = SMAABlendingWeightCalculationPS(texcoord, pixcoord, offsets, edgesTex, areaTex, searchTex, float4(1, 1, 1, 0));
}
