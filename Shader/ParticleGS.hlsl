#include "Particle.hlsli"
#include "Constants.hlsli"

StructuredBuffer<Particle> particleBuffer : register(t9);

Texture2D colorTemperChart : register(t0);
Texture3D<float> noise3d : register(t1);


[maxvertexcount(4)]
void main(point VS_OUT input[1], inout TriangleStream<GS_OUT> output)
{
    const float2 corners[] =
    {
        float2(-1.0, -1.0),
		float2(-1.0, +1.0),
		float2(+1.0, -1.0),
		float2(+1.0, +1.0),
    };
    const float2 texcoords[] =
    {
        float2(0.0, 1.0),
		float2(0.0, 0.0),
		float2(1.0, 1.0),
		float2(1.0, 0.0),
    };
	
    Particle p = particleBuffer[input[0].vertexId];
	
#if 1
    if (p.age <= 0.0 || p.age >= p.lifespan)
    {
        return;
    }
#endif

	
    const float aspectRatio = 1920.0 / 1080.0;

    float ageGaradient = saturate(p.age / p.lifespan);
#if 1
    float size = lerp(p.size.x/*spawn*/, p.size.y/*despawn*/, ageGaradient);
#else	
	float size = p.max_size;
#endif
	
	[unroll]
    for (uint vertexIndex = 0; vertexIndex < 4; ++vertexIndex)
    {
        GS_OUT element;

		// Transform to clip space
        element.position = mul(float4(p.position, 1), sceneData.viewProjection);
		// Make corner position as billboard
        float2 corner;
#if 1
        corner.x = corners[vertexIndex].x * cos(p.angle) - corners[vertexIndex].y * sin(p.angle);
        corner.y = corners[vertexIndex].x * sin(p.angle) + corners[vertexIndex].y * cos(p.angle);
#else
		corner = corners[vertex_index];
#endif
        element.position.xy += corner * float2(size, size * aspectRatio);
		

        //element.color = p.color;
        element.color = emissionColor;
#if 0
		float fade_in_time = 2.0;
		float fade_out_time = 5.0;
		float alpha = fade_in(fade_in_time, p.age, 1) * fade_out(fade_out_time, p.age, p.lifespan, 1);
		element.color.a = pow(alpha, 1);
#endif
		
		
#if 1
        float2 texcoord = texcoords[vertexIndex] / spriteSheetGrid;
        float2 gridSize = 1.0 / spriteSheetGrid;
        uint x = p.chip % spriteSheetGrid.x;
        uint y = p.chip / spriteSheetGrid.x;
        element.texcoord = texcoord + gridSize * uint2(x, y);
#else
		element.texcoord = texcoords[vertex_index];
#endif
		
        output.Append(element);
    }
    output.RestartStrip();
}
