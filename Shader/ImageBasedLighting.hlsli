#ifndef __IMAGE_BASED_LIGHTING__
#define __IMAGE_BASED_LIGHTING__

SamplerState samplerState : register(s0);

Texture2D skybox : register(t32);
TextureCube diffuseIem : register(t33);
TextureCube specularPmrem : register(t34);
Texture2D lutGgx : register(t35);

float4 SampleLutGgx(float2 brdfSamplePoint)
{
    return lutGgx.Sample(samplerState, brdfSamplePoint);
}

float4 SampleSkybox(float3 v,float roughness)
{
    const float PI = 3.14159265358979;
    uint width, height, numberOfLevels;
    skybox.GetDimensions(0, width, height, numberOfLevels);
    
    float lod = roughness * float(numberOfLevels - 1);
    
    v = normalize(v);
    
    //Blinn/Newwell Latitude Mapping
    float2 samplePoint;
    samplePoint.x = (atan2(v.z, v.x) + PI) / (PI * 2.0);
    samplePoint.y = 1.0 - ((asin(v.y) + PI * 0.5) / PI);
    return skybox.SampleLevel(samplerState, samplePoint, lod);
}

float4 SampleDiffuseIem(float3 v)
{
    return diffuseIem.Sample(samplerState, v);
}

float4 SampleSpecularPmrem(float3 v,float roughness)
{
    uint width, height, numberOfLevels;
    specularPmrem.GetDimensions(0, width, height, numberOfLevels);
    
    float lod = roughness * float(numberOfLevels - 1);
    
    return specularPmrem.SampleLevel(samplerState, v, lod);

}

#endif