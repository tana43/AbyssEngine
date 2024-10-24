#include "GeometricSubstance.hlsli"
#include "Constants.hlsli"

VS_OUT main(float3 position : POSITION, float3 normal : NORMAL, float4 tangent : TANGENT, float2 texcoord_0 : TEXCOORD0, float2 texcoord_1 : TEXCOORD1, uint4 joints : JOINTS, float4 weights : WEIGHTS)
{
    if (primitiveData.skin > -1)
    {
        const float w0 = weights.x, w1 = weights.y, w2 = weights.z, w3 = weights.w;
        const float4x4 skinMatrix = w0 * primitiveJointData.matrices[joints.x] + w1 * primitiveJointData.matrices[joints.y] + w2 * primitiveJointData.matrices[joints.z] + w3 * primitiveJointData.matrices[joints.w];
		
        position.xyz = mul(float4(position.xyz, 1), skinMatrix).xyz;
        normal.xyz = normalize(mul(float4(normal.xyz, 0), skinMatrix).xyz);
        tangent.xyz = normalize(mul(float4(tangent.xyz, 0), skinMatrix).xyz);
    }
	
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

