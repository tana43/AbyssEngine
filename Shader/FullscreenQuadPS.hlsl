#include "FullscreenQuad.hlsli"

#define LINEAR_BORDER_BLACK 3
SamplerState samplerStates[5] : register(s0);
Texture2D textureMap : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    return textureMap.Sample(samplerStates[LINEAR_BORDER_BLACK],pin.texcoord);
}