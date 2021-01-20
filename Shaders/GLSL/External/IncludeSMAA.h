#ifdef SHADER_STAGE_FRAGMENT
#define SMAA_INCLUDE_PS 1
#define SMAA_INCLUDE_VS 0
#endif

#ifndef SHADER_STAGE_FRAGMENT
#define SMAA_INCLUDE_PS 0
#define SMAA_INCLUDE_VS 1
#endif

#define SMAA_GLSL_4
#define SMAA_PRESET_ULTRA
#define SMAA_FLIP_Y 0

vec4 SMAA_RT_METRICS;

#include "../External/SMAA.h"