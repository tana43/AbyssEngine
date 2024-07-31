#include "PlaneBillboard.hlsli"
Texture2D texture_ : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    return texture_.Sample(samplerStates[ANISOTROPIC_WRAP],pin.texcoord) * color_;
}