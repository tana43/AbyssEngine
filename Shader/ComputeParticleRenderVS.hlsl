#include "ComputeParticle.hlsli"

GS_IN main( uint vertexId : SV_VertexID )
{
	//’¸“_”Ô†‘—‚é‚¾‚¯
    GS_IN vout;
    vout.vertexId = vertexId;
	return vout;
}