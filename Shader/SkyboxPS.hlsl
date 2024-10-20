#include "FullScreenQuad.hlsli"
#include "ImageBasedLighting.hlsli"
#include "Constants.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 ndc;
    ndc.x = (pin.texcoord.x * 2.0) - 1.0;
    ndc.y = 1.0 - (pin.texcoord.y * 2.0);
    ndc.z = 1;
    ndc.w = 1;
	
    float4 R = mul(ndc, sceneData.inverseViewProjection);
    R /= R.w;
	
    float roughness = sceneData.skyboxRoughness;
    
#if 1
    //return SampleSkybox(R.xyz, roughness) * sceneData.lightColor;
    return SampleSkybox(R.xyz, roughness) * sceneData.skyColor;
#else
	return sample_diffuse_iem(R.xyz);
#endif
}