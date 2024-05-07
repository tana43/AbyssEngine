#include "Common.hlsli"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 wPosition : POSITION;
    float3 wNormal : NORMAL;
    float3 wTangent : TANGENT;
    float3 wBinormal : BINORMAL;
    float2 texcoord_0 : TEXCOORD0;
    float2 texcoord_1 : TEXCOORD1;
};

struct PrimitiveConstants
{
    row_major float4x4 transform;
    float4 color;
    int material;
    int startInstanceLocation; // Index of start of cascade plane
    int skin;
};
cbuffer primitiveConstants : register(b0)
{
    PrimitiveConstants primitiveData;
}

static const uint Primitive_Max_Joints = 512;
struct PrimitiveJointConstants
{
    row_major float4x4 matrices[Primitive_Max_Joints];
};
cbuffer primitiveJointConstants : register(b1)
{
    PrimitiveJointConstants primitiveJointData;
};


#define KHR_texture_transform
#define KHR_materials_clearcoat
#define KHR_materials_specular
#define KHR_materials_sheen

#define KHR_materials_transmission
#define KHR_materials_volume

//#define KHR_materials_unlit
//#define KHR_materials_pbrSpecularGlossiness

