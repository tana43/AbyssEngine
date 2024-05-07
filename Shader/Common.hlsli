#ifndef __COMMON_HLSL__
#define __COMMON_HLSL__

static const float gamma = 2.2;
static const float invGamma = 1.0 / 2.2;

static const float PI = 3.141592653;
static const float epsilon = 0.00001;

float max3(float3 v)
{
    return max(max(v.x, v.y), v.z);
}

#define POINT_WRAP 0
#define LINEAR_WRAP 1
#define ANISOTROPIC_WRAP 2
#define POINT_CLAMP 3
#define LINEAR_CLAMP 4
#define ANISOTROPIC_CLAMP 5
#define POINT_BORDER_OPAQUE_BLACK 6
#define LINEAR_BORDER_OPAQUE_BLACK 7
#define POINT_BORDER_OPAQUE_WHITE 8
#define LINEAR_BORDER_OPAQUE_WHITE 9
#define COMPARISON_DEPTH 10
SamplerState samplerStates[10] : register(s0);
SamplerComparisonState comparisonSamplerState : register(s10);

#endif // __COMMON_HLSL__
