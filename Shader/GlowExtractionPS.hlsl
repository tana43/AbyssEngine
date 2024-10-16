// BLOOM
#include "Bloom.hlsli"

#include "Common.hlsli"

Texture2D hdrColorBufferTexture : register(t0);

float4 main(float4 position : SV_POSITION,float2 texcoord : TEXCOORD) : SV_TARGET
{
    float4 sampledColor = hdrColorBufferTexture.Sample(samplerStates[POINT_WRAP], texcoord);
	
#if 1
	return float4(step(bloomExtractionThreshold,dot(sampledColor.rgb,float3(0.299,0.587,0.114))) * 
	sampledColor.rgb * bloomIntencity,sampledColor.a);
#else
    return float4(step(bloomExtractionThreshold, max(sampledColor.r,max(sampledColor.g, sampledColor.b))) * sampledColor.rgb, sampledColor.a);
#endif
	
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}