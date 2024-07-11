#include "Particle.hlsli"

RWStructuredBuffer<particle> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;

    particle p = particleBuffer[id];
	
    Spawn(id, p);
	
    particleBuffer[id] = p;

}
