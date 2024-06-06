#include "PrimitiveRenderer.hlsli"

VS_OUT main(VS_IN vin)
{
	VS_OUT vout;
	vout.position = mul(vin.position, worldViewProjection);
	vout.color = vin.color;

	return vout;
}
