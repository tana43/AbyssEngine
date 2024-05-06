struct VS_OUT_CSM
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD;
    uint slice : SLICE;
};
struct GS_OUTPUT_CSM
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float depth : DEPTH;
    uint slice : SV_RENDERTARGETARRAYINDEX;
};