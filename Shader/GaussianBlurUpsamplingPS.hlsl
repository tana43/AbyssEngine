//BLOOM
#include "Bloom.hlsli"

#include "Common.hlsli"

static const uint downsampledCount = 6;
Texture2D downsampledTextures[downsampledCount] : register(t0);

float4 main(float4 position : SV_POSITION,float2 texcoord : TEXCOORD) : SV_TARGET
{
    float3 sampledColor = 0;
    [unroll]
    for (uint downsampledIndex = 0; downsampledIndex < downsampledCount; ++downsampledIndex)
    {
        sampledColor += downsampledTextures[downsampledIndex].Sample(samplerStates[LINEAR_WRAP], texcoord).xyz;
    }
    return float4(sampledColor * bloomIntencity,1);
}