#include "GeometricSubstance.hlsli"
#include "CsmOpaque.hlsli"
#include "Constants.hlsli"

VS_OUT_CSM main(float3 position : POSITION, uint instanceId : SV_INSTANCEID)
{
    VS_OUT_CSM vout;

    vout.position = mul(float4(position.xyz, 1), mul(primitiveData.transform, csmData.viewProjectionMatrices[instanceId]));
    vout.slice = primitiveData.startInstanceLocation + instanceId;

    return vout;
}
