#ifndef __IMAGE_BASED_LIGHTING__
#define __IMAGE_BASED_LIGHTING__

#include "Common.hlsli"

#define USE_LATITUDE_LONGITUDE_MAP
#ifdef USE_LATITUDE_LONGITUDE_MAP	
Texture2D skybox : register(t32);
#else
TextureCube skybox : register(t32);
#endif

TextureCube diffuseIem : register(t33);
TextureCube specularPmrem : register(t34);
TextureCube sheenPmrem : register(t35);

Texture2D lutGgx : register(t36);
Texture2D lutSheenE : register(t37);
Texture2D lutCharlie : register(t38);

Texture2D transmissionFramebuffer : register(t39);

float4 SampleLutGgx(float2 brdfSamplePoint)
{
    return lutGgx.Sample(samplerStates[LINEAR_CLAMP], brdfSamplePoint);
}
float AlbedoSheenScalingLut(float NoV, float sheenRoughnessFactor)
{
    return lutSheenE.Sample(samplerStates[LINEAR_CLAMP], float2(NoV, sheenRoughnessFactor)).r;
}
float4 SampleLutCharlie(float2 brdfSamplePoint)
{
    return lutCharlie.Sample(samplerStates[LINEAR_CLAMP], brdfSamplePoint);
}
#if 1
float4 SampleSkybox(float3 v, float roughness, float intensity = 1.0)
{
    uint width, height, numberOfLevels;
    skybox.GetDimensions(0, width, height, numberOfLevels);
	
    float lod = roughness * float(numberOfLevels - 1);
	
#ifdef USE_LATITUDE_LONGITUDE_MAP	
    v = normalize(v);
    // Blinn/Newell Latitude Mapping
    float2 sample_point;
    sample_point.x = (atan2(v.z, v.x) + PI) / (PI * 2.0);
    sample_point.y = 1.0 - ((asin(v.y) + PI * 0.5) / PI);
    return skybox.SampleLevel(samplerStates[LINEAR_CLAMP], sample_point, lod) * intensity;
#else
	return skybox.SampleLevel(samplerStates[LINEAR_CLAMP], v, lod) * intensity;
#endif
}
#endif
float4 SampleDiffuseIem(float3 v, float intensity = 1.0)
{
    return diffuseIem.Sample(samplerStates[LINEAR_CLAMP], v) * intensity;
}
float4 SampleSpecularPmrem(float3 v, float roughness, float intensity = 1.0)
{
    uint width, height, numberOfLevels;
    specularPmrem.GetDimensions(0, width, height, numberOfLevels);
	
    float lod = roughness * float(numberOfLevels - 1);
	
    return specularPmrem.SampleLevel(samplerStates[LINEAR_CLAMP], v, lod) * intensity;
}
float4 SampleSheenPmrem(float3 v, float roughness, float intensity = 1.0)
{
    uint width, height, numberOfLevels;
    specularPmrem.GetDimensions(0, width, height, numberOfLevels);
	
    float lod = roughness * float(numberOfLevels - 1);

    return specularPmrem.SampleLevel(samplerStates[LINEAR_CLAMP], v, lod) * intensity; // TODO
}
float4 SampleTransmission(float2 texcoord, float roughness, float ior)
{
    uint width, height, numberOfLevels;
    transmissionFramebuffer.GetDimensions(0, width, height, numberOfLevels);
	
	// Scale roughness with IOR so that an IOR of 1.0 results in no microfacet refraction and
    // an IOR of 1.5 results in the default amount of microfacet refraction.
#if 0
	float lod = log2(width) * roughness * clamp(ior * 2.0 - 2.0, 0.0, 1.0);
#else
    float lod = numberOfLevels * roughness * clamp(ior * 2.0 - 2.0, 0.0, 1.0);
#endif
	
    return transmissionFramebuffer.SampleLevel(samplerStates[LINEAR_CLAMP], texcoord, lod);
}
#endif // __IMAGE_BASED_LIGHTING__


//#ifndef __IMAGE_BASED_LIGHTING__
//#define __IMAGE_BASED_LIGHTING__

//#include "Common.hlsli"

//Texture2D skybox : register(t32);
//TextureCube diffuseIem : register(t33);
//TextureCube specularPmrem : register(t34);
//Texture2D lutGgx : register(t35);

//float4 SampleLutGgx(float2 brdfSamplePoint)
//{
//    return lutGgx.Sample(samplerStates[LINEAR_CLAMP], brdfSamplePoint);
//}

//float4 SampleSkybox(float3 v,float roughness)
//{
//    const float PI = 3.14159265358979;
//    uint width, height, numberOfLevels;
//    skybox.GetDimensions(0, width, height, numberOfLevels);
    
//    float lod = roughness * float(numberOfLevels - 1);
    
//    v = normalize(v);
    
//    //Blinn/Newwell Latitude Mapping
//    float2 samplePoint;
//    samplePoint.x = (atan2(v.z, v.x) + PI) / (PI * 2.0);
//    samplePoint.y = 1.0 - ((asin(v.y) + PI * 0.5) / PI);
//    return skybox.SampleLevel(samplerStates[LINEAR_CLAMP], samplePoint, lod);
//}

//float4 SampleDiffuseIem(float3 v,float intensity = 1.0)
//{
//    return diffuseIem.Sample(samplerStates[LINEAR_CLAMP], v) * intensity;
//}

//float4 SampleSpecularPmrem(float3 v,float roughness,float intensity = 1.0)
//{
//    uint width, height, numberOfLevels;
//    specularPmrem.GetDimensions(0, width, height, numberOfLevels);
    
//    float lod = roughness * float(numberOfLevels - 1);
    
//    return specularPmrem.SampleLevel(samplerStates[LINEAR_CLAMP], v, lod) * intensity;
//}

//#endif