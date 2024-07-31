#include "Constants.hlsli"
#include "Common.hlsli"

cbuffer ConstantBuffer : register(b0)
{
    row_major matrix worldMatrix;
    float4 color_;
};

struct VS_IN
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};