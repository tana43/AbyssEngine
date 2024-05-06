#include "GeometricSubstance.hlsli"
#include "CsmTransparent.hlsli"
#include "Constants.hlsli"

VS_OUT_CSM main(float3 position : POSITION, float2 texcoord : TEXCOORD, uint instanceId : SV_INSTANCEID)
{
    VS_OUT_CSM vout;

    vout.position = mul(float4(position, 1), mul(primitiveData.transform, csmData.viewProjectionMatrices[instanceId]));
    vout.texcoord = texcoord;
    vout.slice = primitiveData.startInstanceLocation + instanceId;

    return vout;
}
