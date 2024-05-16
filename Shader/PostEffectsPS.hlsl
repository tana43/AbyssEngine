#include "FullscreenQuad.hlsli"
#include "Constants.hlsli"
#include "Common.hlsli"

#if 0
Texture2D colorMap : register(t0);
Texture2D depthMap : register(t1);
Texture2DArray shadowMap : register(t2);
Texture2D bloomMap : register(t3);
Texture2D volumetricLightBuffer : register(t4);

Texture2D noise2dMap : register(t30);
Texture3D noise3dMap : register(t31);

// Poisson Disk PCF sampling
#define MAX_POISSON_DISC_SAMPLES 64
static const float2 poissonSamples[MAX_POISSON_DISC_SAMPLES] =
{
    float2(-0.5119625f, -0.4827938f),
	float2(-0.2171264f, -0.4768726f),
	float2(-0.7552931f, -0.2426507f),
	float2(-0.7136765f, -0.4496614f),
	float2(-0.5938849f, -0.6895654f),
	float2(-0.3148003f, -0.7047654f),
	float2(-0.42215f, -0.2024607f),
	float2(-0.9466816f, -0.2014508f),
	float2(-0.8409063f, -0.03465778f),
	float2(-0.6517572f, -0.07476326f),
	float2(-0.1041822f, -0.02521214f),
	float2(-0.3042712f, -0.02195431f),
	float2(-0.5082307f, 0.1079806f),
	float2(-0.08429877f, -0.2316298f),
	float2(-0.9879128f, 0.1113683f),
	float2(-0.3859636f, 0.3363545f),
	float2(-0.1925334f, 0.1787288f),
	float2(0.003256182f, 0.138135f),
	float2(-0.8706837f, 0.3010679f),
	float2(-0.6982038f, 0.1904326f),
	float2(0.1975043f, 0.2221317f),
	float2(0.1507788f, 0.4204168f),
	float2(0.3514056f, 0.09865579f),
	float2(0.1558783f, -0.08460935f),
	float2(-0.0684978f, 0.4461993f),
	float2(0.3780522f, 0.3478679f),
	float2(0.3956799f, -0.1469177f),
	float2(0.5838975f, 0.1054943f),
	float2(0.6155105f, 0.3245716f),
	float2(0.3928624f, -0.4417621f),
	float2(0.1749884f, -0.4202175f),
	float2(0.6813727f, -0.2424808f),
	float2(-0.6707711f, 0.4912741f),
	float2(0.0005130528f, -0.8058334f),
	float2(0.02703013f, -0.6010728f),
	float2(-0.1658188f, -0.9695674f),
	float2(0.4060591f, -0.7100726f),
	float2(0.7713396f, -0.4713659f),
	float2(0.573212f, -0.51544f),
	float2(-0.3448896f, -0.9046497f),
	float2(0.1268544f, -0.9874692f),
	float2(0.7418533f, -0.6667366f),
	float2(0.3492522f, 0.5924662f),
	float2(0.5679897f, 0.5343465f),
	float2(0.5663417f, 0.7708698f),
	float2(0.7375497f, 0.6691415f),
	float2(0.2271994f, -0.6163502f),
	float2(0.2312844f, 0.8725659f),
	float2(0.4216993f, 0.9002838f),
	float2(0.4262091f, -0.9013284f),
	float2(0.2001408f, -0.808381f),
	float2(0.149394f, 0.6650763f),
	float2(-0.09640376f, 0.9843736f),
	float2(0.7682328f, -0.07273844f),
	float2(0.04146584f, 0.8313184f),
	float2(0.9705266f, -0.1143304f),
	float2(0.9670017f, 0.1293385f),
	float2(0.9015037f, -0.3306949f),
	float2(-0.5085648f, 0.7534177f),
	float2(0.9055501f, 0.3758393f),
	float2(0.7599946f, 0.1809109f),
	float2(-0.2483695f, 0.7942952f),
	float2(-0.4241052f, 0.5581087f),
	float2(-0.1020106f, 0.6724468f),
};

float4 Permute(float4 x)
{
    return fmod(((x * 34.0) + 1.0) * x, 289.0);
}
float4 TaylorInvSqrt(float4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

float Snoise(float3 v)
{
    const float2 C = float2(1.0 / 6.0, 1.0 / 3.0);
    const float4 D = float4(0.0, 0.5, 1.0, 2.0);

	// First corner
    float3 i = floor(v + dot(v, C.yyy));
    float3 x0 = v - i + dot(i, C.xxx);

	// Other corners
    float3 g = step(x0.yzx, x0.xyz);
    float3 l = 1.0 - g;
    float3 i1 = min(g.xyz, l.zxy);
    float3 i2 = max(g.xyz, l.zxy);

	//  x0 = x0 - 0. + 0.0 * C 
    float3 x1 = x0 - i1 + 1.0 * C.xxx;
    float3 x2 = x0 - i2 + 2.0 * C.xxx;
    float3 x3 = x0 - 1. + 3.0 * C.xxx;

	// Permutations
    i = fmod(i, 289.0);
    float4 p = Permute(Permute(Permute(
             i.z + float4(0.0, i1.z, i2.z, 1.0))
           + i.y + float4(0.0, i1.y, i2.y, 1.0))
           + i.x + float4(0.0, i1.x, i2.x, 1.0));

	// Gradients
	// ( N*N points uniformly over a square, mapped onto an octahedron.)
    float n_ = 1.0 / 7.0; // N=7
    float3 ns = n_ * D.wyz - D.xzx;

    float4 j = p - 49.0 * floor(p * ns.z * ns.z); //  mod(p,N*N)

    float4 x_ = floor(j * ns.z);
    float4 y_ = floor(j - 7.0 * x_); // mod(j,N)

    float4 x = x_ * ns.x + ns.yyyy;
    float4 y = y_ * ns.x + ns.yyyy;
    float4 h = 1.0 - abs(x) - abs(y);

    float4 b0 = float4(x.xy, y.xy);
    float4 b1 = float4(x.zw, y.zw);

    float4 s0 = floor(b0) * 2.0 + 1.0;
    float4 s1 = floor(b1) * 2.0 + 1.0;
    float4 sh = -step(h, 0.0);

    float4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
    float4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

    float3 p0 = float3(a0.xy, h.x);
    float3 p1 = float3(a0.zw, h.y);
    float3 p2 = float3(a1.xy, h.z);
    float3 p3 = float3(a1.zw, h.w);

	//Normalise gradients
    float4 norm = TaylorInvSqrt(float4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

	// Mix final noise value
    float4 m = max(0.6 - float4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0);
    m = m * m;
    return 42.0 * dot(m * m, float4(dot(p0, x0), dot(p1, x1), dot(p2, x2), dot(p3, x3)));
}
float3 ApplyBokeh(Texture2D colorMap, float aspect /*H div W*/, float depthNdc, float2 texcoord, float aperture, float focus, float max_blur)
{
	//Bokeh Effect
	//Max Blur: The maximum amount of blurring.Ranges from 0 to 1
	//Aperture: Bigger values create a shallower depth of field
	//Focus: Controls the focus of the effect
	//Aspect: Controls the blurring effect

	//float aspect = dimensions.y / dimensions.x;
    const float2 aspect_correct = float2(1.0, aspect);

    const float factor = depthNdc - focus;
    const float2 dofblur = clamp(factor * aperture, -max_blur, max_blur);
    const float2 dofblur9 = dofblur * 0.9;
    const float2 dofblur7 = dofblur * 0.7;
    const float2 dofblur4 = dofblur * 0.4;

    float4 color = 0;
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.0, 0.4) * aspect_correct) * dofblur);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.15, 0.37) * aspect_correct) * dofblur);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.29, 0.29) * aspect_correct) * dofblur);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(-0.37, 0.15) * aspect_correct) * dofblur);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.40, 0.0) * aspect_correct) * dofblur);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.37, -0.15) * aspect_correct) * dofblur);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.29, -0.29) * aspect_correct) * dofblur);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(-0.15, -0.37) * aspect_correct) * dofblur);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.0, -0.4) * aspect_correct) * dofblur);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(-0.15, 0.37) * aspect_correct) * dofblur);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(-0.29, 0.29) * aspect_correct) * dofblur);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.37, 0.15) * aspect_correct) * dofblur);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(-0.4, 0.0) * aspect_correct) * dofblur);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(-0.37, -0.15) * aspect_correct) * dofblur);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(-0.29, -0.29) * aspect_correct) * dofblur);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.15, -0.37) * aspect_correct) * dofblur);

    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.15, 0.37) * aspect_correct) * dofblur9);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(-0.37, 0.15) * aspect_correct) * dofblur9);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.37, -0.15) * aspect_correct) * dofblur9);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(-0.15, -0.37) * aspect_correct) * dofblur9);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(-0.15, 0.37) * aspect_correct) * dofblur9);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.37, 0.15) * aspect_correct) * dofblur9);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(-0.37, -0.15) * aspect_correct) * dofblur9);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.15, -0.37) * aspect_correct) * dofblur9);

    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.29, 0.29) * aspect_correct) * dofblur7);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.40, 0.0) * aspect_correct) * dofblur7);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.29, -0.29) * aspect_correct) * dofblur7);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.0, -0.4) * aspect_correct) * dofblur7);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(-0.29, 0.29) * aspect_correct) * dofblur7);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(-0.4, 0.0) * aspect_correct) * dofblur7);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(-0.29, -0.29) * aspect_correct) * dofblur7);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.0, 0.4) * aspect_correct) * dofblur7);

    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.29, 0.29) * aspect_correct) * dofblur4);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.4, 0.0) * aspect_correct) * dofblur4);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.29, -0.29) * aspect_correct) * dofblur4);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.0, -0.4) * aspect_correct) * dofblur4);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(-0.29, 0.29) * aspect_correct) * dofblur4);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(-0.4, 0.0) * aspect_correct) * dofblur4);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(-0.29, -0.29) * aspect_correct) * dofblur4);
    color += colorMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], texcoord + (float2(0.0, 0.4) * aspect_correct) * dofblur4);

    return color.rgb / 41.0;
}

float3 ApplyShadow(inout float3 color, in float4 positionWorldSpace, in float depthViewSpace, in float2 shadowMapDimensions, in float3 randSeed)
{
    float shadowFactor = 0.0;
	
	// Find a layer of cascaded view frustum volume 
    int cascadeIndex = -1;
    for (uint layer = 0; layer < 4; ++layer)
    {
        float distance = csmData.cascadePlaneDistances[layer];
        if (distance > depthViewSpace)
        {
            cascadeIndex = layer;
            break;
        }
    }
	// outside far panel
    if (cascadeIndex == -1)
    {
        return color;
    }
	
	// world space to light view clip space, and to ndc
    float4 positionLightSpace = mul(positionWorldSpace, csmData.viewProjectionMatrices[cascadeIndex]);
    positionLightSpace /= positionLightSpace.w;
	// ndc to texture space
    positionLightSpace.x = positionLightSpace.x * +0.5 + 0.5;
    positionLightSpace.y = positionLightSpace.y * -0.5 + 0.5;

#if 0
	// Hard shadows
	shadowFactor = shadowMap.SampleCmpLevelZero(comparisonSamplerState, float3(positionLightSpace.xy, cascadeIndex), positionLightSpace.z - effectData.shadowDepthBias).x;
#else
	// Soft shadows
    const float2 sampleScale = (0.5 * effectData.shadowFilterRadius) / shadowMapDimensions;
    float accum = 0.0;
    for (uint sampleIndex = 0; sampleIndex < effectData.shadowSampleCount; ++sampleIndex)
    {
        float2 sampleOffset;
        float4 seed = float4(randSeed, sampleIndex);
        uint random = (uint) (64.0 * frac(sin(dot(seed, float4(12.9898, 78.233, 45.164, 94.673))) * 43758.5453)) % 64;
        sampleOffset = poissonSamples[random] * sampleScale;

        float2 samplePosition = positionLightSpace.xy + sampleOffset;
        accum += shadowMap.SampleCmpLevelZero(comparisonSamplerState, float3(samplePosition, cascadeIndex), positionLightSpace.z - effectData.shadowDepthBias).x;
    }
    shadowFactor = accum / effectData.shadowSampleCount;
#endif
	
#if 1
    if (effectData.colorizeCascadedLayer)
    {
        const float3 colors[4] =
        {
            { 1, 0, 0 },
            { 0, 1, 0 },
            { 0, 0, 1 },
            { 1, 1, 0 },
        };
        return color * lerp(effectData.shadowColor, 1.0, shadowFactor) * colors[cascadeIndex];
    }
#endif
	
    return color * lerp(effectData.shadowColor, 1.0, shadowFactor);
}


float CalculateShadowsAndLighting(float depthViewSpace, float3 positionWorldSpace)
{
    int cascadeIndex = -1;
    for (uint layer = 0; layer < 4; ++layer)
    {
        float distance = csmData.cascadePlaneDistances[layer];
        if (distance > depthViewSpace)
        {
            cascadeIndex = layer;
            break;
        }
    }
    if (cascadeIndex == -1)
    {
        return 1;
    }
    float4 positionLightSpace = mul(float4(positionWorldSpace, 1.0), csmData.viewProjectionMatrices[cascadeIndex]);
    positionLightSpace /= positionLightSpace.w;

	// To texture space
    positionLightSpace.x = positionLightSpace.x * +0.5 + 0.5;
    positionLightSpace.y = positionLightSpace.y * -0.5 + 0.5;
	
    return shadowMap.SampleCmpLevelZero(comparisonSamplerState, float3(positionLightSpace.xy, cascadeIndex), positionLightSpace.z - effectData.shadowDepthBias).x;
}
// Mie scaterring approximated with Henyey-Greenstein phase function.
// https://developer.nvidia.com/gpugems/gpugems2/part-ii-shading-lighting-and-shadows/chapter-16-accurate-atmospheric-scattering
#define PI 3.14159265358979
float ComputeMieScattering(float LoV)
{
    const float g = effectData.mieG;
    float mieScattering = 1.0 - g * g;
    mieScattering /= (4.0 * PI * pow(1.0 + g * g - (2.0 * g) * LoV, 1.5));
    return mieScattering;
}
float3 ComputeLayleighScattering(float LoV)
{
    return 3 / (16.0 * PI) * (1.0 + LoV * LoV) * float3(5.8, 13.5, 33.1) * 1e-2;
}
// https://www.alexandre-pestana.com/volumetric-lights/
float3 ApplyVolumetricFog(inout float3 color, in float3 positionWorldSpace)
{
    float3 startPosition = sceneData.eyePosition.xyz;
	
    const float3 rayVector = positionWorldSpace - startPosition;
    const float rayLength = length(rayVector);
    const float3 rayDirection = rayVector / rayLength;
	
    const float3 L = normalize(-sceneData.lightDirection.xyz);
    const float LoV = max(0, dot(rayDirection, L));

    const float steps = 64;
    const float stepLength = rayLength / steps;
    const float3 step = rayDirection * stepLength;

	
#if 0
	const float4x4 dither_pattern =
	{
		{ 0.0f, 0.5f, 0.125f, 0.625f },
		{ 0.75f, 0.22f, 0.875f, 0.375f },
		{ 0.1875f, 0.6875f, 0.0625f, 0.5625 },
		{ 0.9375f, 0.4375f, 0.8125f, 0.3125 }
	};
	// Offset the start position
	startPosition += dither_pattern[positionWorldSpace.x % 4][positionWorldSpace.y % 4];
#endif
	
    float3 currentPositionWorldSpace = startPosition;
    float currentDepthViewSpace = 0;
    float accumulatedFog = 0;
	
	// Volumetric fog computation
    for (int i = 0; i < steps; i++)
    {
        float currentFogAmount = 0.0f;
		
        float noise = 1.0;
        const float3 windVelocity = float3(1.0, 0.0, 0.0);
#if 0
		noise = (1.0 + Snoise((currentPositionWorldSpace.xyz + windVelocity * sceneData.time) * effectData.noiseScale)) * 0.5;
#else
        noise = 0.5 * noise3dMap.Sample(samplerStates[LINEAR_WRAP], (currentPositionWorldSpace.xyz + windVelocity * sceneData.time) * effectData.noiseScale).x + 0.5;
#endif
		
        float litAmount = CalculateShadowsAndLighting(currentDepthViewSpace, currentPositionWorldSpace);
		
		// Exponential height fog
        float b = effectData.fogHeightFalloff;
        float c = effectData.fogDensity * noise;
        float t = stepLength * c * exp(-currentPositionWorldSpace.y * b);
        float vy = b * step.y * stepLength;
        currentFogAmount = litAmount * max(0.0, 1.0 - exp(t / vy * (exp(-vy) - 1.0)));
		
		// Mie and Layleigh Scattering
        const float scatteringIntensity = 0.02;
        const float scattering = litAmount * (ComputeMieScattering(LoV) /* + ComputeLayleighScattering(LoV)*/) * scatteringIntensity;
		
        accumulatedFog += lerp(currentFogAmount, scattering, 0.5);
		
		// Extend the ray by a step in the ray direction
        currentPositionWorldSpace += step;
        currentDepthViewSpace += stepLength;
    }

    const float maxFogOpacity = 0.75;
    accumulatedFog = min(maxFogOpacity, accumulatedFog);
	
    float3 fogColor = effectData.fogColor.rgb * effectData.fogColor.w;
#if 1
    float3 sunColor = float3(1.0, 0.9, 0.7) /* * sceneData.pure_white*/;
    float3 sunDirection = L;
    float sunAmount = max(0.0, dot(rayDirection, sunDirection));
    fogColor = lerp(fogColor, sunColor, pow(sunAmount, 128.0));
#endif

    return lerp(color, fogColor, saturate(accumulatedFog));
}

float4 main(VS_OUT pin) : SV_TARGET
{
    uint mipLevel = 0, numberOfSamples, levels;
	
    uint2 colorMapDimensions;
    colorMap.GetDimensions(mipLevel, colorMapDimensions.x, colorMapDimensions.y, numberOfSamples);
    const float aspect = (float) colorMapDimensions.y / colorMapDimensions.x;

    uint2 shadowMapDimensions;
    shadowMap.GetDimensions(mipLevel, shadowMapDimensions.x, shadowMapDimensions.y, numberOfSamples, levels);

    uint2 depthMapDimensions;
    depthMap.GetDimensions(mipLevel, depthMapDimensions.x, depthMapDimensions.y, numberOfSamples);

	
#if 0
	float3 noise = noise3dMap.Sample(samplerStates[LINEAR_WRAP], float3(pin.texcoord.xy, sceneData.time * 0.05)).xxx;
	return float4(0.5 * noise + 0.5, 1);
#endif	
	
    float4 sampledColor = colorMap.Sample(samplerStates[POINT_CLAMP], pin.texcoord);
    float3 color = sampledColor.rgb;
    float alpha = sampledColor.a;

    float depthNdc = depthMap.Sample(samplerStates[POINT_CLAMP], pin.texcoord).x;

    float4 positionNdc;
	// texture space to ndc
    positionNdc.x = pin.texcoord.x * +2 - 1;
    positionNdc.y = pin.texcoord.y * -2 + 1;
    positionNdc.z = depthNdc;
    positionNdc.w = 1;

	// ndc to view space
    float4 positionViewSpace = mul(positionNdc, sceneData.inverseProjection);
    positionViewSpace = positionViewSpace / positionViewSpace.w;
	
	// ndc to world space
    float4 positionWorldSpace = mul(positionNdc, sceneData.inverseViewProjection);
	
#if 0
	// Apply bokeh effect
    color = ApplyBokeh(colorMap, aspect, depthNdc, pin.texcoord, effectData.bokehAperture, effectData.bokehFocus, 0.1);
#endif

#if 1
	// Apply cascade shadow mapping
    color = ApplyShadow(color, positionWorldSpace, positionViewSpace.z, shadowMapDimensions, positionNdc.xyz);
#endif
	
#if 0
	// Apply volumetric lighting
	//color = ApplyVolumetricFog(color, positionWorldSpace.xyz);
	return volumetricLightBuffer.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], pin.texcoord);
#endif

	// Accelerating Unstructured Volume Rendering with Joint Bilateral Upsampling
	// http://www.sci.utah.edu/~stevec/papers/bilateral/index.html
    float currDepth = depthMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], pin.texcoord).x;
    float4 sum = float4(0.0, 0.0, 0.0, 0.0);
    float4 sample;
    float radius = 4.0;
    float2 pos;
    float i, j;
    float sigma = 2.0 * radius * radius;
    float domainGaussian = 0.0;
    float weight = 0.0;
    float distance = 0.0;
    float sampleDepth;
    float rangeGaussian;
    float sigma2 = 0.01;
    for (i = -radius; i <= radius; i += 1.0)
    {
        for (j = -radius; j <= radius; j += 1.0)
        {
            float dx = i / depthMapDimensions.x;
            float dy = j / depthMapDimensions.y;
            pos.x = pin.texcoord.x + dx;
            pos.y = pin.texcoord.y + dy;
            sample = volumetricLightBuffer.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], pos);

            distance = i * i + j * j;
            domainGaussian = exp(-distance / sigma);
			
            sampleDepth = depthMap.Sample(samplerStates[LINEAR_BORDER_OPAQUE_BLACK], pos).x;
            distance = (currDepth - sampleDepth) * (currDepth - sampleDepth);
            rangeGaussian = exp(-distance / sigma2);
			
            sum += sample * domainGaussian * rangeGaussian;
            weight += domainGaussian * rangeGaussian;
        }
    }
    float4 volumetricLightColor = sum / weight;
	
	//return volumetricLightColor;
	
    //color = color * volumetricLightColor.a + volumetricLightColor.rgb;
	//color = color + volumetricLightColor.rgb;
	
#if 1
	// Apply bloom effect
    float3 bloomColor = bloomMap.Sample(samplerStates[LINEAR_CLAMP], pin.texcoord).rgb;
    color += bloomColor;
#endif
	
	
    return float4(color, alpha);
}
#else
Texture2D colorMap : register(t0);
Texture2D depthMap : register(t1);
Texture2DArray shadowMap : register(t2);
Texture2D bloomMap : register(t3);

// Poisson Disk PCF sampling
#define MAX_POISSON_DISC_SAMPLES 64
static const float2 poissonSamples[MAX_POISSON_DISC_SAMPLES] =
{
    float2(-0.5119625f, -0.4827938f),
	float2(-0.2171264f, -0.4768726f),
	float2(-0.7552931f, -0.2426507f),
	float2(-0.7136765f, -0.4496614f),
	float2(-0.5938849f, -0.6895654f),
	float2(-0.3148003f, -0.7047654f),
	float2(-0.42215f, -0.2024607f),
	float2(-0.9466816f, -0.2014508f),
	float2(-0.8409063f, -0.03465778f),
	float2(-0.6517572f, -0.07476326f),
	float2(-0.1041822f, -0.02521214f),
	float2(-0.3042712f, -0.02195431f),
	float2(-0.5082307f, 0.1079806f),
	float2(-0.08429877f, -0.2316298f),
	float2(-0.9879128f, 0.1113683f),
	float2(-0.3859636f, 0.3363545f),
	float2(-0.1925334f, 0.1787288f),
	float2(0.003256182f, 0.138135f),
	float2(-0.8706837f, 0.3010679f),
	float2(-0.6982038f, 0.1904326f),
	float2(0.1975043f, 0.2221317f),
	float2(0.1507788f, 0.4204168f),
	float2(0.3514056f, 0.09865579f),
	float2(0.1558783f, -0.08460935f),
	float2(-0.0684978f, 0.4461993f),
	float2(0.3780522f, 0.3478679f),
	float2(0.3956799f, -0.1469177f),
	float2(0.5838975f, 0.1054943f),
	float2(0.6155105f, 0.3245716f),
	float2(0.3928624f, -0.4417621f),
	float2(0.1749884f, -0.4202175f),
	float2(0.6813727f, -0.2424808f),
	float2(-0.6707711f, 0.4912741f),
	float2(0.0005130528f, -0.8058334f),
	float2(0.02703013f, -0.6010728f),
	float2(-0.1658188f, -0.9695674f),
	float2(0.4060591f, -0.7100726f),
	float2(0.7713396f, -0.4713659f),
	float2(0.573212f, -0.51544f),
	float2(-0.3448896f, -0.9046497f),
	float2(0.1268544f, -0.9874692f),
	float2(0.7418533f, -0.6667366f),
	float2(0.3492522f, 0.5924662f),
	float2(0.5679897f, 0.5343465f),
	float2(0.5663417f, 0.7708698f),
	float2(0.7375497f, 0.6691415f),
	float2(0.2271994f, -0.6163502f),
	float2(0.2312844f, 0.8725659f),
	float2(0.4216993f, 0.9002838f),
	float2(0.4262091f, -0.9013284f),
	float2(0.2001408f, -0.808381f),
	float2(0.149394f, 0.6650763f),
	float2(-0.09640376f, 0.9843736f),
	float2(0.7682328f, -0.07273844f),
	float2(0.04146584f, 0.8313184f),
	float2(0.9705266f, -0.1143304f),
	float2(0.9670017f, 0.1293385f),
	float2(0.9015037f, -0.3306949f),
	float2(-0.5085648f, 0.7534177f),
	float2(0.9055501f, 0.3758393f),
	float2(0.7599946f, 0.1809109f),
	float2(-0.2483695f, 0.7942952f),
	float2(-0.4241052f, 0.5581087f),
	float2(-0.1020106f, 0.6724468f),
};

float3 ApplyShadow(inout float3 color, in float4 positionWorldSpace, in float depthViewSpace, in float2 shadowMapDimensions, in float3 randSeed)
{
    float shadowFactor = 0.0;

	// Find a layer of cascaded view frustum volume 
    int cascadeIndex = -1;
    for (uint layer = 0; layer < csmData.cascadeCount; ++layer)
    {
        float distance = ((float[4]) (csmData.cascadePlaneDistances[layer / 4]))[layer % 4];
        if (distance > depthViewSpace)
        {
            cascadeIndex = layer;
            break;
        }
    }
	// outside far panel
    if (cascadeIndex == -1)
    {
        return color;
    }

	// world space to light view clip space, and to ndc
    float4 positionLightSpace = mul(positionWorldSpace, csmData.viewProjectionMatrices[cascadeIndex]);
    positionLightSpace /= positionLightSpace.w;
	// ndc to texture space
    positionLightSpace.x = positionLightSpace.x * +0.5 + 0.5;
    positionLightSpace.y = positionLightSpace.y * -0.5 + 0.5;

#if 0
	// Hard shadows
	shadowFactor = shadowMap.SampleCmpLevelZero(comparisonSamplerState, float3(positionLightSpace.xy, cascadeIndex), positionLightSpace.z - effectData.shadowDepthBias).x;
#else
	// Soft shadows
    const float2 sampleScale = (0.5 * effectData.shadowFilterRadius) / shadowMapDimensions;
    float accum = 0.0;
    for (uint sampleIndex = 0; sampleIndex < effectData.shadowSampleCount; ++sampleIndex)
    {
        float2 sampleOffset;
        float4 seed = float4(randSeed, sampleIndex);
        uint random = (uint) (64.0 * frac(sin(dot(seed, float4(12.9898, 78.233, 45.164, 94.673))) * 43758.5453)) % 64;
        sampleOffset = poissonSamples[random] * sampleScale;

        float2 samplePosition = positionLightSpace.xy + sampleOffset;
        accum += shadowMap.SampleCmpLevelZero(comparisonSamplerState, float3(samplePosition, cascadeIndex), positionLightSpace.z - effectData.shadowDepthBias).x;
    }
    shadowFactor = accum / effectData.shadowSampleCount;
#endif

#if 1
    if (effectData.colorizeCascadedLayer)
    {
        const float3 colors[4] =
        {
            { 1, 0, 0 },
            { 0, 1, 0 },
            { 0, 0, 1 },
            { 1, 1, 0 },
        };
        return color * lerp(effectData.shadowColor, 1.0, shadowFactor) * colors[cascadeIndex];
    }
#endif

    return color * lerp(effectData.shadowColor, 1.0, shadowFactor);
}

float4 main(VS_OUT pin) : SV_TARGET
{    
    uint2 dimensions;
    uint mipLevel = 0, numberOfSamples;
    colorMap.GetDimensions(mipLevel, dimensions.x, dimensions.y, numberOfSamples);
    const float aspect = (float) dimensions.y / dimensions.x;

    uint2 shadowMapDimensions;
    uint shadowMapMipLevel = 0, shadowMapNumberOfSamples, levels;
    shadowMap.GetDimensions(shadowMapMipLevel, shadowMapDimensions.x, shadowMapDimensions.y, shadowMapNumberOfSamples, levels);

    float4 sampledColor = colorMap.Sample(samplerStates[POINT_CLAMP], pin.texcoord);
    float3 color = sampledColor.rgb;
    float alpha = sampledColor.a;

    float depthNdc = depthMap.Sample(samplerStates[POINT_CLAMP], pin.texcoord).x;

    float4 positionNdc;
	// texture space to ndc
    positionNdc.x = pin.texcoord.x * +2 - 1;
    positionNdc.y = pin.texcoord.y * -2 + 1;
    positionNdc.z = depthNdc;
    positionNdc.w = 1;

	// ndc to view space
    float4 positionViewSpace = mul(positionNdc, sceneData.inverseProjection);
    positionViewSpace = positionViewSpace / positionViewSpace.w;
	
	// ndc to world space
    float4 positionWorldSpace = mul(positionNdc, sceneData.inverseViewProjection);
    positionWorldSpace = positionWorldSpace / positionWorldSpace.w;

	// Apply cascade shadow mapping
    color = ApplyShadow(color, positionWorldSpace, positionViewSpace.z, shadowMapDimensions, positionNdc.xyz);

	// Apply bloom effect
    float3 bloomColor = bloomMap.Sample(samplerStates[LINEAR_CLAMP], pin.texcoord).rgb;
    color += bloomColor;

    return float4(color, alpha);
}
#endif
