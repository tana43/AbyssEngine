#include "GeometricSubstance.hlsli"
#include "Constants.hlsli"

VS_OUT main(float3 position : POSITION, float3 normal : NORMAL, float4 tangent : TANGENT, float2 texcoord_0 : TEXCOORD0, float2 texcoord_1 : TEXCOORD1)
{
    VS_OUT vout;

    vout.position = mul(float4(position.xyz, 1), mul(primitiveData.transform, sceneData.viewProjection));
    vout.wPosition = mul(float4(position.xyz, 1), primitiveData.transform).xyz;

    vout.wNormal = normalize(mul(float4(normal.xyz, 0), primitiveData.transform).xyz);

    float sigma = tangent.w;
    vout.wTangent = normalize(mul(float4(tangent.xyz, 0), primitiveData.transform).xyz);
    vout.wBinormal = cross(vout.wNormal, vout.wTangent) * sigma;
	
    vout.texcoord_0 = texcoord_0;
    vout.texcoord_1 = texcoord_1;
	
    return vout;
}
