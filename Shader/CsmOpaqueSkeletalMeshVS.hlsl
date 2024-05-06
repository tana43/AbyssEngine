#include "GeometricSubstance.hlsli"
#include "CsmOpaque.hlsli"
#include "constants.hlsli"

VS_OUT_CSM main(float3 position : POSITION, uint4 joints : JOINTS, float4 weights : WEIGHTS, uint instance_id : SV_INSTANCEID)
{
    VS_OUT_CSM vout;

    if (primitiveData.skin > -1)
    {
        const float w0 = weights.x, w1 = weights.y, w2 = weights.z, w3 = weights.w;
        const float4x4 skin_matrix = w0 * primitiveJointData.matrices[joints.x] + w1 * primitiveJointData.matrices[joints.y] + w2 * primitiveJointData.matrices[joints.z] + w3 * primitiveJointData.matrices[joints.w];
        position = mul(float4(position.xyz, 1), skin_matrix).xyz;
    }
	
    vout.position = mul(float4(position.xyz, 1), mul(primitive_data.transform, csm_data.view_projection_matrices[instance_id]));
    vout.slice = primitiveData.start_instance_location + instance_id;

    return vout;
}
