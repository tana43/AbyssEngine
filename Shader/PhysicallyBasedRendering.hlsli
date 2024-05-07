#ifndef __PHYSICALLY_BASED_RENDERING__
#define __PHYSICALLY_BASED_RENDERING__

#include "Material.hlsli"
#include "BidirectionalReflectanceDistributionFunction.hlsli"

// This fragment shader defines a reference implementation for Physically Based Shading of
// a microfacet surface material defined by a glTF model.
//
// References:
// [1] Real Shading in Unreal Engine 4
//     http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
// [2] Physically Based Shading at Disney
//     http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf
// [3] README.md - Environment Maps
//     https://github.com/KhronosGroup/glTF-WebGL-PBR/#environment-maps
// [4] "An Inexpensive BRDF Model for Physically based Rendering" by Christophe Schlick
//     https://www.cs.virginia.edu/~jdl/bib/appearance/analytic%20models/schlick94b.pdf
// [5] "KHR_materials_clearcoat"
//     https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_clearcoat

float4 PhysicallyBasedRendering(MaterialInfo materialInfo, float3 L, float3 V, float3 N, float3 P, float3 Li, float4x4 world, float4x4 viewProjection, float emissiveIntensity, float imageBasedLightingIntensity)
{
    float3 fSpecular = 0.0;
    float3 fDiffuse = 0.0;
    float3 fEmissive = 0.0;
    float3 fClearcoat = 0.0;
    float3 fSheen = 0.0;
    float3 fTransmission = 0.0;

    float albedoSheenScaling = 1.0;
	
    // IBL
    if (imageBasedLightingIntensity > 0)
    {
        fDiffuse += IblRadianceLambertian(N, V, materialInfo.perceptualRoughness, materialInfo.cDiff, materialInfo.f0, materialInfo.specularWeight, imageBasedLightingIntensity);
		
        fSpecular += IblRadianceGgx(N, V, materialInfo.perceptualRoughness, materialInfo.f0, materialInfo.specularWeight, imageBasedLightingIntensity);

#ifdef KHR_materials_clearcoat
		fClearcoat += IblRadianceGgx(materialInfo.clearcoatNormal, V, materialInfo.clearcoatRoughnessFactor, materialInfo.clearcoatF0, 1.0, imageBasedLightingIntensity);
#endif	
		//return float4(fClearcoat, 1);
	
#ifdef KHR_materials_sheen
		fSheen += IblRadianceCharlie(N, V, materialInfo.sheenRoughnessFactor, materialInfo.sheenColorFactor, imageBasedLightingIntensity);
#endif

#if (defined(KHR_materials_transmission) || defined(KHR_materials_volume))
		fTransmission += materialInfo.transmissionFactor * IblVolumeRefraction(
			N, V,
			materialInfo.perceptualRoughness,
			materialInfo.cDiff, materialInfo.f0, materialInfo.f90,
			P, world, viewProjection,
			materialInfo.ior, materialInfo.thicknessFactor, materialInfo.attenuationColor, materialInfo.attenuationDistance);
#endif		
    }

	
	// BSTF
	//float3 R = reflect(-L, N);
    float3 H = normalize(V + L);
    
    float NoL = clamp(dot(N, L), 0.0, 1.0);
    float NoH = clamp(dot(N, H), 0.0, 1.0);
    float VoH = clamp(dot(V, H), 0.0, 1.0);
#if 0
	float NoV = dot(N, V);
#else
    float NoV = max(1e-4, dot(N, V)); // Neubelt and Pettineo 2013, "Crafting a Next-gen Material Pipeline for The Order: 1886"
#endif
	
    if (NoL > 0.0 || NoV > 0.0)
    {
        // Calculation of analytical light
        // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#acknowledgments AppendixB
        fDiffuse += Li * NoL * BrdfLambertian(materialInfo.f0, materialInfo.f90, materialInfo.cDiff, materialInfo.specularWeight, VoH);
        fSpecular += Li * NoL * BrdfSpecularGgx(materialInfo.f0, materialInfo.f90, materialInfo.alphaRoughness, materialInfo.specularWeight, VoH, NoL, NoV, NoH);

#ifdef KHR_materials_sheen
		//fSheen += Li * NoL * brdfSpecular_sheen(materialInfo.sheen_color_factor, materialInfo.sheen_roughness_factor, NoL, NoV, NoH);
		//albedoSheenScaling = min(1.0 - max3(materialInfo.sheen_color_factor) * albedoSheenScaling_lut(NoV, materialInfo.sheen_roughness_factor),
		//	1.0 - max3(materialInfo.sheen_color_factor) * albedoSheenScaling_lut(NoL, materialInfo.sheen_roughness_factor));
#endif

#ifdef KHR_materials_clearcoat
		//fClearcoat += Li * PunctualRadianceClearCoat(materialInfo.clearcoatNormal, V, L, H, VoH,
		//	materialInfo.clearcoatF0, materialInfo.clearcoatF90, materialInfo.clearcoatRoughnessFactor);
#endif
    }
	
	// BDTF
#ifdef KHR_materials_transmission
    // If the light ray travels through the geometry, use the point it exits the geometry again.
    // That will change the angle to the light source, if the material refracts the light ray.
	//float3 transmissionRay = VolumeTransmissionRay(N, V, materialInfo.thicknessFactor, materialInfo.ior, world);
	//L = normalize(L - transmissionRay);

	//float3 transmittedLight = Li * PunctualRadianceTransmission(N, V, L, materialInfo.alpha_roughness, materialInfo.f0, materialInfo.f90, materialInfo.cDiff, materialInfo.ior);

#ifdef KHR_materials_volume
	//transmittedLight = apply_volume_attenuation(transmittedLight, length(transmissionRay), materialInfo.attenuation_color, materialInfo.attenuation_distance);
#endif
	
	//fTransmission += materialInfo.transmission_factor * transmittedLight;
#endif
	
    // Apply ambient occlusion to all liwghting that is not punctual
    fDiffuse = lerp(fDiffuse, fDiffuse * materialInfo.occlusionFactor, materialInfo.occlusionStrength);
    fSpecular = lerp(fSpecular, fSpecular * materialInfo.occlusionFactor, materialInfo.occlusionStrength);
    fSheen = lerp(fSheen, fSheen * materialInfo.occlusionFactor, materialInfo.occlusionStrength);
    fClearcoat = lerp(fClearcoat, fClearcoat * materialInfo.occlusionFactor, materialInfo.occlusionStrength);
	
	// Apply emissive factor
    fEmissive = materialInfo.emissiveFactor * materialInfo.emissiveStrength;
#if 1
    fEmissive *= emissiveIntensity;
#endif
	
    float3 color = 0.0;
	
	// Layer blending
    float clearcoatFactor = 0;
    float3 clearcoatFresnel = 0;
#ifdef KHR_materials_clearcoat
	//clearcoatFactor = materialInfo.clearcoatFactor;
	//clearcoatFresnel = f_schlick(materialInfo.clearcoatF0, materialInfo.clearcoatF90, clamp(dot(materialInfo.clearcoatNormal, V), 0.0, 1.0));
	//fClearcoat = fClearcoat * clearcoatFactor;
#endif
	
#ifdef KHR_materials_transmission
	//fDiffuse = lerp(fDiffuse, fTransmission, materialInfo.transmission_factor);
#endif

    color = fEmissive + fDiffuse + fSpecular;
    color = fSheen + color * albedoSheenScaling;
    color = color * (1.0 - clearcoatFactor * clearcoatFresnel) + fClearcoat;
    return float4(max(0, color), materialInfo.basecolor.a);
    
    //return color;
}

#endif // __PHYSICALLY_BASED_RENDERING__