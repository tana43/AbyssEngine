#include "ComputeParticle.hlsli"

GS_IN main( uint vertexId : SV_VertexID )
{
	//頂点番号送るだけ
    GS_IN vout;
    vout.vertexId = vertexId;
	return vout;
}