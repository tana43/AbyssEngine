#include "ComputeParticle.hlsli"

GS_IN main( uint vertexId : SV_VertexID )
{
	//���_�ԍ����邾��
    GS_IN vout;
    vout.vertexId = vertexId;
	return vout;
}