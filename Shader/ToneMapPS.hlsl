#include "FullscreenQuad.hlsli"

#include "Constants.hlsli"
#include "Common.hlsli"

Texture2D textures[2] : register(t0);

// sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
column_major const static matrix<float, 3, 3> aces_input_mat =
{
    0.59719, 0.07600, 0.02840,
    0.35458, 0.90834, 0.13383,
    0.04823, 0.01566, 0.83777
};

// ODT_SAT => XYZ => D60_2_D65 => sRGB
column_major const static matrix<float, 3, 3> aces_output_mat =
{
    1.60475, -0.10208, -0.00327,
    -0.53108, 1.10813, -0.07276,
    -0.07367, -0.00605, 1.07602
};

// ACES tone map (faster approximation)
// see: https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 TonemapAcesNarkowicz(float3 color)
{
    const float A = 2.51;
    const float B = 0.03;
    const float C = 2.43;
    const float D = 0.59;
    const float E = 0.14;
    return clamp((color * (A * color + B)) / (color * (C * color + D) + E), 0.0, 1.0);
}

// ACES filmic tone map approximation
// see https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl
float3 RrtAndOdtFit(float3 color)
{
    float3 a = color * (color + 0.0245786) - 0.000090537;
    float3 b = color * (0.983729 * color + 0.4329510) + 0.238081;
    return a / b;
}

// tone mapping 
float3 TonemapAcesHill(float3 color)
{
    color = mul(color, aces_input_mat);

    // Apply RRT and ODT
    color = RrtAndOdtFit(color);

    color = mul(color, aces_output_mat);

    // Clamp to [0, 1]
    color = clamp(color, 0.0, 1.0);

    return color;
}

#define TonemapAcesNarkowicz
//#define TonemapAcesHill
//#define TonemapAcesHill_EXPOSURE_BOOST
float3 Tonemap(float3 color)
{
    color *= sceneData.exposure;

#ifdef TonemapAcesNarkowicz
    color = TonemapAcesNarkowicz(color);
#endif

#ifdef TonemapAcesHill
    color = TonemapAcesHill(color);
#endif

#ifdef TonemapAcesHill_EXPOSURE_BOOST
    // boost exposure as discussed in https://github.com/mrdoob/three.js/pull/19621
    // this factor is based on the exposure correction of Krzysztof Narkowicz in his
    // implemetation of ACES tone mapping
    color /= 0.6;
    color = TonemapAcesHill(color);
#endif

    return color;
}

// https://www.geeks3d.com/20110405/fxaa-fast-approximate-anti-aliasing-demo-glsl-opengl-test-radeon-geforce/
#define FXAA_REDUCE_MIN (1.0/128.0)
#define FXAA_REDUCE_MUL (1.0/8.0)
#define FXAA_SPAN_MAX 8.0
#define FXAA_SUBPIX_SHIFT (1.0 / 4.0)
#define FXAA_SAMPLER LINEAR_CLAMP
float3 Fxaa(Texture2D framebuffer, float4 posPos, float2 rcpFrame)
{
    float3 sampledNw = framebuffer.Sample(samplerStates[FXAA_SAMPLER], posPos.zw).xyz;
    float3 sampledNe = framebuffer.Sample(samplerStates[FXAA_SAMPLER], posPos.zw, int2(1, 0)).xyz;
    float3 sampledSw = framebuffer.Sample(samplerStates[FXAA_SAMPLER], posPos.zw, int2(0, 1)).xyz;
    float3 sampledSe = framebuffer.Sample(samplerStates[FXAA_SAMPLER], posPos.zw, int2(1, 1)).xyz;
    float3 sampledM = framebuffer.Sample(samplerStates[FXAA_SAMPLER], posPos.xy).xyz;

    float3 luma = float3(0.299, 0.587, 0.114);
    float lumaNw = dot(sampledNw, luma);
    float lumaNe = dot(sampledNe, luma);
    float lumaSw = dot(sampledSw, luma);
    float lumaSe = dot(sampledSe, luma);
    float lumaM = dot(sampledM, luma);

    float lumaMin = min(lumaM, min(min(lumaNw, lumaNe), min(lumaSw, lumaSe)));
    float lumaMax = max(lumaM, max(max(lumaNw, lumaNe), max(lumaSw, lumaSe)));

    float2 dir;
    dir.x = -((lumaNw + lumaNe) - (lumaSw + lumaSe));
    dir.y = ((lumaNw + lumaSw) - (lumaNe + lumaSe));

    float dirReduce = max((lumaNw + lumaNe + lumaSw + lumaSe) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(float2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), max(float2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * rcpFrame;

    float3 rgbA = (1.0 / 2.0) * (
		framebuffer.Sample(samplerStates[FXAA_SAMPLER], posPos.xy + dir * (1.0 / 3.0 - 0.5)).xyz +
		framebuffer.Sample(samplerStates[FXAA_SAMPLER], posPos.xy + dir * (2.0 / 3.0 - 0.5)).xyz);
    float3 rgbB = rgbA * (1.0 / 2.0) + (1.0 / 4.0) * (
		framebuffer.Sample(samplerStates[FXAA_SAMPLER], posPos.xy + dir * (0.0 / 3.0 - 0.5)).xyz +
		framebuffer.Sample(samplerStates[FXAA_SAMPLER], posPos.xy + dir * (3.0 / 3.0 - 0.5)).xyz);
    float lumaB = dot(rgbB, luma);

    return ((lumaB < lumaMin) || (lumaB > lumaMax)) ? rgbA : rgbB;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    uint mipLevel = 0, width, height, number_of_levels;
    textures[0].GetDimensions(mipLevel, width, height, number_of_levels);
	
#if 0
	float4 sampledColor = textures[0].Sample(samplerStates[POINT_WRAP], pin.texcoord);
#else
	// FXAA(Fast approximate anti-aliasing)
    float2 rcpFrame = float2(1.0 / width, 1.0 / height);
    float4 posPos;
    posPos.xy = pin.texcoord.xy;
    posPos.zw = pin.texcoord.xy - (rcpFrame * (0.5 + FXAA_SUBPIX_SHIFT));

    float4 sampledColor = 1.0;
    sampledColor.rgb = Fxaa(textures[0], posPos, rcpFrame);
#endif	
	
	// Apply tone mapping.
    sampledColor.rgb = Tonemap(sampledColor.rgb);
  
	// Apply gamma correction
    sampledColor.rgb = pow(sampledColor.rgb, invGamma);

    return sampledColor;
}
