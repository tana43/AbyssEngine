struct VS_OUT_CSM
{
    float4 position : POSITION;
    uint slice : SLICE;
};

struct GS_OUTPUT_CSM
{
    float4 position : SV_POSITION;
    uint slice : SV_RENDERTARGETARRAYINDEX;
};