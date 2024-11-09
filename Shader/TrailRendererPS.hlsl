#include "TrailRenderer.hlsli"
#include "Common.hlsli"

Texture2D tex2d : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 col = pin.color * color;
    //return tex2d.Sample(samplerStates[ANISOTROPIC_WRAP],pin.texcoord) * col * intensity;
    return tex2d.Sample(samplerStates[ANISOTROPIC_WRAP],pin.texcoord) * color * intensity;
}