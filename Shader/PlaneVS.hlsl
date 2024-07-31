#include "PlaneBillboard.hlsli"

VS_OUT main( VS_IN vin )
{
    VS_OUT output = (VS_OUT)0;
    float4 pos = float4(vin.position, 1.0f);
    output.position = mul(pos, worldMatrix);
    output.position = mul(output.position, sceneData.viewProjection);
    output.texcoord = vin.texcoord;
    return output;
}