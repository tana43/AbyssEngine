#include "FullscreenQuad.hlsli"
#include "Constants.hlsli"
#include "Common.hlsli"

Texture2D tex : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float3 color = float3(0.0f, 0.0f, 0.0f);
    
    //ブラーの中心とするUVまでの距離を算出
    float2 uvOffset = effectData.radialBlurUvOffset;
    float2 uv = pin.texcoord - uvOffset;
    
    float strength = effectData.radialBlurStrength;
    float sampleCount = effectData.radialBlurSampleCount;
    
    //UVを程度ずらす移動値を算出
    float factor = strength / sampleCount * length(uv);
	
    for (int i = 0; i < sampleCount; i++)
    {
        float offset = 1 - factor * i;
        color += tex.Sample(samplerStates[LINEAR_CLAMP],
            uv * offset + uvOffset).rgb;
    }
    
    //値を１以内にするためにサンプル数で除算
    return float4(color/sampleCount , 1.0f);
}