#include "GeometricSubstance.hlsli"

#include "Material.hlsli"
#include "PhysicallyBasedRendering.hlsli"

#include "Constants.hlsli"
#include "Common.hlsli"

float3 compute_normal(float3 position)
{
    return normalize(cross(ddx(position), ddy(position)));
}
float3 compute_tangent(float3 position, float3 normal, float2 uv)
{
    float3 uv_dx = ddx(float3(uv, 0.0));
    float3 uv_dy = ddy(float3(uv, 0.0));

    if (length(uv_dx) + length(uv_dy) <= 1e-6)
    {
        uv_dx = float3(1.0, 0.0, 0.0);
        uv_dy = float3(0.0, 1.0, 0.0);
    }
    float3 tangent = (uv_dy.y * ddx(position) - uv_dx.y * ddy(position)) / ((uv_dx.x * uv_dy.y - uv_dy.x * uv_dx.y) + 1e-15);
    return normalize(tangent - normal * dot(normal, tangent));
}

#if 0
// If you enable earlydepthstencil, 'clip', 'discard' and 'Alpha to coverage' won't work!
[earlydepthstencil]
#endif
float4 main(VS_OUT pin, bool is_front_face : SV_ISFRONTFACE) : SV_TARGET
{
    const float3 P = pin.wPosition.xyz;

    float2 texcoords[2] = { pin.texcoord_0, pin.texcoord_1 };
    const MaterialConstants material = materials[primitiveData.material];
    MaterialInfo materialInfo = MakeMaterialInfo(material, texcoords);

    if (material.alphaMode == ALPHAMODE_OPAQUE)
    {
        materialInfo.basecolor.a = 1.0;
    }

#if 0
	float alpha_cutoff = material.alpha_cutoff;
	if (!any(material_info.emissive_factor))
	{
		clip(material_info.basecolor.a - alpha_cutoff);
	}
#else
	// Late discard to avoid samplig artifacts. See https://github.com/KhronosGroup/glTF-Sample-Viewer/issues/267
    if (!any(materialInfo.emissiveFactor) && materialInfo.basecolor.a < material.alphaCutoff)
    {
        discard;
    }
#endif
	


#ifdef KHR_materials_unlit	
	return material_info.basecolor;
#endif
	
    float3 V = normalize(sceneData.eyePosition.xyz - pin.wPosition.xyz);
    float3 L = normalize(-sceneData.lightDirection.xyz);
	
    float3 N = normalize(pin.wNormal);
    float3 T = normalize(pin.wTangent);
    float3 B = normalize(pin.wBinormal);

#if 0
	// If you did not have normal, we can calculate it in pixel shader.
	N = compute_normal(P);
#endif
#if 0
	// If you did not have tangent, we can calculate it in pixel shader.
	T = compute_tangent(P, N, texcoords[material.normal_texture.texcoord]);
	B = normalize(cross(N, T));
#endif

#if 0
	// For a back-facing surface, the tangential basis vectors are negated.
	if (is_front_face == false)
	{
		N = -N;
		T = -T;
		B = -B;
	}
#endif

    const int normal_texture = material.normalTexture.index;
    if (normal_texture > -1)
    {
        float2 transformedTexcoord = TransformTexcoord(texcoords[material.normalTexture.texcoord], material.normalTexture.khrTextureTransform);
        float4 sampled = materialTextures[NORMAL_TEXTURE].Sample(samplerStates[LINEAR_WRAP], transformedTexcoord);
        float3 normalFactor = sampled.xyz;
        normalFactor = (normalFactor * 2.0) - 1.0;
        normalFactor = normalize(normalFactor * float3(material.normalTexture.scale, material.normalTexture.scale, 1.0));
        N = normalize((normalFactor.x * T) + (normalFactor.y * B) + (normalFactor.z * N));
    }

#ifdef KHR_materials_clearcoat
	materialInfo.clearcoatNormal = normalize(pin.wNormal.xyz);
	const int clearcoatNormalTexture = material.khrMaterialsClearcoat.clearcoatNormalTexture.index;
	if (clearcoatNormalTexture > -1)
	{
		float2 transformedTexcoord = TransformTexcoord(texcoords[material.khrMaterialsClearcoat.clearcoatNormalTexture.texcoord], material.khrMaterialsClearcoat.clearcoatNormalTexture.khrTextureTransform);
		float4 sampled = materialTextures[CLEARCOAT_NORMAL_TEXTURE].Sample(samplerStates[LINEAR_WRAP], transformedTexcoord);
		float3 normalFactor = sampled.xyz;
		normalFactor = (normalFactor * 2.0) - 1.0;
		normalFactor = normalize(normalFactor * float3(material.khrMaterialsClearcoat.clearcoatNormalTexture.scale, material.khrMaterialsClearcoat.clearcoatNormalTexture.scale, 1.0));
		materialInfo.clearcoatNormal = normalize((normalFactor.x * T) + (normalFactor.y * B) + (normalFactor.z * N));
	}
#endif
    
    float3 lightColor = sceneData.lightColor.xyz;
    float3 Li = lightColor * sceneData.pureWhite;
    return PhysicallyBasedRendering(materialInfo, L, V, N, P, Li, primitiveData.transform, sceneData.viewProjection, primitiveData.emissiveIntensity, primitiveData.imageBasedLightingIntensity);
}


