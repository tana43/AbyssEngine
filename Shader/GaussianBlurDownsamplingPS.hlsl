//BLOOM
#include "Common.hlsli"
Texture2D hbrColorBufferTexture : register(t0);

float4 main(float4 position : SV_POSITION,float2 texcoord : TEXCOORD) : SV_TARGET
{
    return hbrColorBufferTexture.Sample(samplerStates[LINEAR_WRAP],texcoord,0.0);
}