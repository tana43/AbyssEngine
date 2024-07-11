#include "particle.hlsli"


RWStructuredBuffer<Particle> particleBuffer : register(u0);

Texture2D colorTemperChart : register(t0);
Texture3D<float> noise3d : register(t1);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;

    Particle p = particleBuffer[id];
	
    p.age += deltaTime;

    bool respawn = true;
    if (p.age > p.lifespan && respawn) // Lifespan has expired.
    {
        Spawn(id, p);
    }

    if (p.age > 0)
    {
	
#if 0
		float3 wind_dir = float3(1, 0, 0);
		float wind_strength = 1;
		p.velocity += normalize(wind_dir) * wind_strength * delta_time;
#endif
	
		
        float3 force = 0;
	
		// gravity
        force += p.mass * float3(0.0, gravity, 0.0);
		
#if 0
		// vector field
		float3 electric_charge_position = float3(sin(time), 1.0, cos(time));
		
		float3 r = electric_charge_position - p.position;
		float w = 1.0;

		float l = max(1e-4, length(r));
		force += w * normalize(r) / (l * l * l);
#endif

        p.velocity += force / p.mass * deltaTime;
#if 1		
        float speed = length(p.velocity);
        const float maxSpeed = 10.0;
        p.velocity = min(maxSpeed, speed) * normalize(p.velocity);
#endif	
        p.position += p.velocity * deltaTime;
		
		
        p.color = colorTemperChart.SampleLevel(samplerStates[LINEAR_WRAP], float2(0.5, p.age / p.lifespan), 0);
        p.color = pow(p.color, 2.3);
#if 1
        float alpha = FadeIn(fadeDuration.x, p.age, 1) * FadeOut(fadeDuration.y, p.age, p.lifespan, 1);
        p.color.a = pow(alpha, 1);
#endif		
		
        p.angle += p.angularSpeed * deltaTime;
    }


    particleBuffer[id] = p;
}
