#include "FullscreenQuad.hlsli"
#include "Constants.hlsli"
#include "Common.hlsli"

Texture2D tex : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float3 color = float3(0.0f, 0.0f, 0.0f);
    
    //�u���[�̒��S�Ƃ���UV�܂ł̋������Z�o
    float2 uvOffset = effectData.radialBlurUvOffset;
    float2 uv = pin.texcoord - uvOffset;
    
    float strength = effectData.radialBlurStrength;
    float sampleCount = effectData.radialBlurSampleCount;
    
    //UV����x���炷�ړ��l���Z�o
    float factor = strength / sampleCount * length(uv);
	
    for (int i = 0; i < sampleCount; i++)
    {
        float offset = 1 - factor * i;
        color += tex.Sample(samplerStates[LINEAR_CLAMP],
            uv * offset + uvOffset).rgb;
    }
    
    //�l���P�ȓ��ɂ��邽�߂ɃT���v�����ŏ��Z
    return float4(color/sampleCount , 1.0f);
}