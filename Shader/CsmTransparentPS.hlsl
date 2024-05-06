#include "CsmTransparent.hlsli"
#include "Common.hlsli"

Texture2D colorMap : register(t1);

float main(GS_OUTPUT_CSM pin) : SV_DEPTH
{
    float alphaCutoff = 0.5;
    float alpha = colorMap.Sample(samplerStates[ANISOTROPIC_WRAP], pin.texcoord).a;
    clip(alpha - alphaCutoff);
    return pin.depth;
}
