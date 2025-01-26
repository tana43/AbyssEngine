#include "SwordTrailRenderer.hlsli"
#include "Constants.hlsli"

VS_OUT main(VS_IN vin)
{
    VS_OUT vout;
    vout.position = mul(vin.position, sceneData.viewProjection);
    vout.color = vin.color;
    vout.texcoord = vin.texcoord;
	
    return vout;
}