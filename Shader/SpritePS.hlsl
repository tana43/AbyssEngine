#include "Common.hlsli"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

Texture2D tex2d : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    return tex2d.Sample(samplerStates[ANISOTROPIC_WRAP], pin.texcoord) * pin.color;
}