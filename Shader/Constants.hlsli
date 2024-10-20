#ifndef __CONSTANTS__
#define __CONSTANTS__

struct SceneConstants
{
    row_major float4x4 view;
    row_major float4x4 projection;
    row_major float4x4 viewProjection;
    row_major float4x4 inverseView;
    row_major float4x4 inverseProjection;
    row_major float4x4 inverseViewProjection;
    float4 lightDirection;
    float4 lightColor;
    float4 eyePosition;
    float4 focusPosition;
    float exposure;
    float pureWhite;
    float skyboxRoughness;
    float time;
};
cbuffer sceneConstants : register(b10)
{
    SceneConstants sceneData;
}

struct EffectConstants
{
    float4 fogColor;
    float fogDensity;
    float fogHeightFalloff;
    float groundLevel;
    float maxRayLength;
    float mieG;
    float noiseScale;
    float timeScale;
	
    float bokehAperture;
    float bokehFocus;
	
    float shadowDepthBias;
    float shadowColor;
    float shadowFilterRadius;
    uint shadowSampleCount;
    uint colorizeCascadedLayer;
    
    float radialBlurStrength;
    uint radialBlurSampleCount;
    float2 radialBlurUvOffset;
};
cbuffer effectConstants : register(b11)
{
    EffectConstants effectData;
}

struct CsmConstants
{
    row_major float4x4 viewProjectionMatrices[4];
    float4 cascadePlaneDistances;
    uint cascadeCount;
};
cbuffer csmConstants : register(b12)
{
    CsmConstants csmData;
}

#endif // __CONSTANTS__