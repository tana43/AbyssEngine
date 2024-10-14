#include "CsmTransparent.hlsli"

VS_OUT_CSM main(float4 pos : POSITION)
{
    VS_OUT_CSM vout;
    vout.position = pos;
    vout.texcoord = float2(0, 0);
    vout.slice = 0;
	return vout;
}