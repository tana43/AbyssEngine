#ifndef __BIDIRECTIONAL_REFLECTANCE_DISTRIBUTION_FUNCTION_HLSL__
#define __BIDIRECTIONAL_REFLECTANCE_DISTRIBUTION_FUNCTION_HLSL__

#include "ImageBasedLighting.hlsli"

// Fresnel
// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
// https://github.com/wdas/brdf/tree/master/src/brdfs
// https://google.github.io/filament/Filament.md.html
// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
float3 FSchlick(float3 f0, float3 f90, float VoH)
{
    return f0 + (f90 - f0) * pow(clamp(1.0 - VoH, 0.0, 1.0), 5.0);
}
// Smith Joint GGX
// Note: Vis = G / (4 * NoL * NoV)
// see Eric Heitz. 2014. Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs. Journal of Computer Graphics Techniques, 3
// see Real-Time Rendering. Page 331 to 336.
// see https://google.github.io/filament/Filament.md.html#materialsystem/specularbrdf/geometricshadowing(specularg)
float VGgx(float NoL, float NoV, float alphaRoughness)
{
    float alphaRoughnessSq = alphaRoughness * alphaRoughness;

    float ggxv = NoL * sqrt(NoV * NoV * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);
    float ggxl = NoV * sqrt(NoL * NoL * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);

    float ggx = ggxv + ggxl;
    return (ggx > 0.0) ? 0.5 / ggx : 0.0;
}
// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
// Implementation from "Average Irregularity Representation of a Roughened Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
float DGgx(float NoH, float alphaRoughness)
{
    float alphaRoughnessSq = alphaRoughness * alphaRoughness;
    float f = (NoH * NoH) * (alphaRoughnessSq - 1.0) + 1.0;
    return alphaRoughnessSq / (PI * f * f);
}

//  https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#acknowledgments AppendixB
float3 BrdfSpecularGgx(float3 f0, float3 f90, float alphaRoughness, float specularWeight, float VoH, float NoL, float NoV, float NoH)
{
    float3 F = FSchlick(f0, f90, VoH);
    float Vis = VGgx(NoL, NoV, alphaRoughness);
    float D = DGgx(NoH, alphaRoughness);

    return specularWeight * F * Vis * D;
}
float3 PunctualRadianceClearCoat(float3 N /*clearcoat_normal*/, float3 V, float3 L, float3 H, float VoH, float3 f0, float3 f90, float clearcoatRoughness)
{
    float NoL = clamp(dot(N, L), 0.0, 1.0);
    float NoV = clamp(dot(N, V), 0.0, 1.0);
    float NoH = clamp(dot(N, H), 0.0, 1.0);
    return NoL * BrdfSpecularGgx(f0, f90, clearcoatRoughness * clearcoatRoughness, 1.0, VoH, NoL, NoV, NoH);
}

//https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#acknowledgments AppendixB
float3 BrdfLambertian(float3 f0, float3 f90, float3 diffuseColor, float specularWeight, float VoH)
{
    // see https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
    return (1.0 - specularWeight * FSchlick(f0, f90, VoH)) * (diffuseColor / PI);
}

// Estevez and Kulla http://www.aconty.com/pdf/s2017_pbs_imageworks_sheen.pdf
float DCharlie(float sheenRoughness, float NoH)
{
    sheenRoughness = max(sheenRoughness, 0.000001); //clamp (0,1]
    float alpha = sheenRoughness * sheenRoughness;
    float inv_alpha = 1.0 / alpha;
    float cos2h = NoH * NoH;
    float sin2h = 1.0 - cos2h;
    return (2.0 + inv_alpha) * pow(sin2h, inv_alpha * 0.5) / (2.0 * PI);
}
float LambdaSheenNumericHelper(float x, float alpha)
{
    float oneMinusAlphaSq = (1.0 - alpha) * (1.0 - alpha);
    float a = lerp(21.5473, 25.3245, oneMinusAlphaSq);
    float b = lerp(3.82987, 3.32435, oneMinusAlphaSq);
    float c = lerp(0.19823, 0.16801, oneMinusAlphaSq);
    float d = lerp(-1.97760, -1.27393, oneMinusAlphaSq);
    float e = lerp(-4.32054, -4.85967, oneMinusAlphaSq);
    return a / (1.0 + b * pow(x, c)) + d * x + e;
}
float lambda_sheen(float cosTheta, float alpha)
{
    if (abs(cosTheta) < 0.5)
    {
        return exp(LambdaSheenNumericHelper(cosTheta, alpha));
    }
    else
    {
        return exp(2.0 * LambdaSheenNumericHelper(0.5, alpha) - LambdaSheenNumericHelper(1.0 - cosTheta, alpha));
    }
}
float VSheen(float NoL, float NoV, float sheenRoughness)
{
    sheenRoughness = max(sheenRoughness, 0.000001); //clamp (0,1]
    float alpha = sheenRoughness * sheenRoughness;

    return clamp(1.0 / ((1.0 + lambda_sheen(NoV, alpha) + lambda_sheen(NoL, alpha)) * (4.0 * NoV * NoL)), 0.0, 1.0);
}
float3 BrdfSpecularSheen(float3 sheenColor, float sheenRoughness, float NoL, float NoV, float NoH)
{
    float sheenDistribution = DCharlie(sheenRoughness, NoH);
    float sheenVisibility = VSheen(NoL, NoV, sheenRoughness);
    return sheenColor * sheenDistribution * sheenVisibility;
}

float3 IblRadianceGgx(float3 N, float3 V, float roughness, float3 f0, float specularWeight, float intensity)
{
    float NoV = clamp(dot(N, V), 0.0, 1.0);
	
    float2 brdfSamplePoint = clamp(float2(NoV, roughness), 0.0, 1.0);
    float2 fAb = SampleLutGgx(brdfSamplePoint).rg;

    float3 R = normalize(reflect(-V, N));
    float3 specularLight = SampleSpecularPmrem(R, roughness, intensity).rgb;

    // see https://bruop.github.io/ibl/#single_scattering_results at Single Scattering Results
    // Roughness dependent fresnel, from Fdez-Aguera
    float3 fr = max(1.0 - roughness, f0) - f0;
    float3 kS = f0 + fr * pow(1.0 - NoV, 5.0);
    float3 fssEss = kS * fAb.x + fAb.y;

    return specularWeight * specularLight * fssEss;
}
// specularWeight is introduced with KHR_materials_specular
float3 IblRadianceLambertian(float3 N, float3 V, float roughness, float3 diffuseColor, float3 f0, float specularWeight, float intensity)
{
    float NoV = clamp(dot(N, V), 0.0, 1.0);
	
    float2 brdfSamplePoint = clamp(float2(NoV, roughness), 0.0, 1.0);
    float2 fAb = SampleLutGgx(brdfSamplePoint).rg;

    float3 irradiance = SampleDiffuseIem(N, intensity).rgb;
	
    // see https://bruop.github.io/ibl/#single_scattering_results at Single Scattering Results
    // Roughness dependent fresnel, from Fdez-Aguera
    float3 fr = max(1.0 - roughness, f0) - f0;
    float3 kS = f0 + fr * pow(1.0 - NoV, 5.0);
    float3 fssEss = specularWeight * kS * fAb.x + fAb.y; // <--- GGX / specular light contribution (scale it down if the specularWeight is low)

    // Multiple scattering, from Fdez-Aguera
    float ems = (1.0 - (fAb.x + fAb.y));
    float3 fAvg = specularWeight * (f0 + (1.0 - f0) / 21.0);
    float3 fmsEms = ems * fssEss * fAvg / (1.0 - fAvg * ems);
    float3 kD = diffuseColor * (1.0 - fssEss + fmsEms); // we use +FmsEms as indicated by the formula in the blog post (might be a typo in the implementation)

    return (fmsEms + kD) * irradiance;
}
float3 IblRadianceCharlie(float3 N, float3 V, float sheenRoughness, float3 sheenColor, float intensity)
{
    float NoV = clamp(dot(N, V), 0.0, 1.0);

    float2 brdfSamplePoint = clamp(float2(NoV, sheenRoughness), 0.0, 1.0);
    float brdf = SampleLutCharlie(brdfSamplePoint).b;
	
    float3 R = normalize(reflect(-V, N));
    float3 sheenSample = SampleSheenPmrem(R, sheenRoughness, intensity).rgb; // TODO

    float3 sheenLight = sheenSample.rgb;
    return sheenLight * sheenColor * brdf;
}

float3 VolumeTransmissionRay(float3 N, float3 V, float thickness, float ior, column_major float4x4 modelMatrix)
{
    // Direction of refracted light.
    float3 refractionVector = refract(-V, normalize(N), 1.0 / ior);

    // Compute rotation-independant scaling of the model matrix.
    float3 model_scale;
#if 0
	model_scale.x = length(float3(modelMatrix[0].xyz));
	model_scale.y = length(float3(modelMatrix[1].xyz));
	model_scale.z = length(float3(modelMatrix[2].xyz));
#else
    model_scale.x = length(mul(float4(1, 0, 0, 0), modelMatrix));
    model_scale.y = length(mul(float4(0, 1, 0, 0), modelMatrix));
    model_scale.z = length(mul(float4(0, 0, 1, 0), modelMatrix));
#endif

    // The thickness is specified in local space.
    return normalize(refractionVector) * thickness * model_scale;
}
// Compute attenuated light as it travels through a volume.
float3 ApplyVolumeAttenuation(float3 radiance, float transmissionDistance, float3 attenuationColor, float attenuationDistance)
{
    if (attenuationDistance == 0.0)
    {
        // Attenuation distance is +infinity (which we indicate by zero), i.e. the transmitted color is not attenuated at all.
        return radiance;
    }
    else
    {
        // Compute light attenuation using Beer's law.
        float3 attenuation_coefficient = -log(attenuationColor) / attenuationDistance;
        float3 transmittance = exp(-attenuation_coefficient * transmissionDistance); // Beer's law
        return transmittance * radiance;
    }
}
float3 IblVolumeRefraction(float3 N, float3 V, float perceptualRoughness, float3 basecolor, float3 f0, float3 f90,
    float3 P, column_major float4x4 worldTransform, column_major float4x4 viewProjectionTransform, float ior, float thickness, float3 attenuationColor, float attenuationDistance)
{
    float3 transmissionRay = VolumeTransmissionRay(N, V, thickness, ior, worldTransform);
    float3 refractedRayExit = P + transmissionRay;

    // Project refracted vector on the framebuffer, while mapping to normalized device coordinates.
    float4 ndc = mul(float4(refractedRayExit, 1.0), viewProjectionTransform);
    ndc = ndc / ndc.w;
	
    float2 refractionCoords;
    refractionCoords.x = 0.5 + 0.5 * ndc.x;
    refractionCoords.y = 0.5 - 0.5 * ndc.y;

    // Sample framebuffer to get pixel the refracted ray hits.
    float3 transmitted_light = SampleTransmission(refractionCoords, perceptualRoughness, ior).rgb;
    float3 attenuated_color = ApplyVolumeAttenuation(transmitted_light, length(transmissionRay), attenuationColor, attenuationDistance);

    // Sample GGX LUT to get the specular component.
    float NoV = clamp(dot(N, V), 0.0, 1.0);
    float2 brdfSamplePoint = clamp(float2(NoV, perceptualRoughness), 0.0, 1.0);
    float2 brdf = SampleLutGgx(brdfSamplePoint).rg;
    float3 specular_color = f0 * brdf.x + f90 * brdf.y;

    return (1.0 - specular_color) * attenuated_color * basecolor;
}
float3 PunctualRadianceTransmission(float3 N, float3 V, float3 L, float alphaRoughness, float3 f0, float3 f90, float3 basecolor, float ior)
{
    float transmissionRougness = alphaRoughness * clamp(ior * 2.0 - 2.0, 0.0, 1.0);

    N = normalize(N); // Outward direction of surface point
    V = normalize(V); // Direction from surface point to view
    L = normalize(L);
    float3 l_mirror = normalize(L + 2.0 * N * dot(-L, N)); // Mirror light reflection vector on surface
    float3 H = normalize(l_mirror + V); // Halfway vector between transmission light vector and v

    float D = DGgx(clamp(dot(N, H), 0.0, 1.0), transmissionRougness);
    float3 F = FSchlick(f0, f90, clamp(dot(V, H), 0.0, 1.0));
    float Vis = VGgx(clamp(dot(N, l_mirror), 0.0, 1.0), clamp(dot(N, V), 0.0, 1.0), transmissionRougness);

    // Transmission BTDF
    return (1.0 - F) * basecolor * D * Vis;
}
#endif // __BIDIRECTIONAL_REFLECTANCE_DISTRIBUTION_FUNCTION_HLSL__
/*
#ifndef __BIDIRECTIONAL_REFLECTANCE_DISTRIBUTION_FUNCTION_HLSL__
#define __BIDIRECTIONAL_REFLECTANCE_DISTRIBUTION_FUNCTION_HLSL__

#include "ImageBasedLighting.hlsli"

float3 FSchlick(float3 f0,float3 f90,float VoH)
{
    return f0 + (f90 - f0) * pow(clamp(1.0 - VoH, 0.0, 1.0), 5.0);
}

float Vggx(float NoL, float NoV, float alphaRoughness)
{
    float alphaRoughnessSq = alphaRoughness * alphaRoughness;
    
    float ggxv = NoL * sqrt(NoV * NoV * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);
    float ggxl = NoV * sqrt(NoL * NoL * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);

    float ggx = ggxv + ggxl;
    return (ggx > 0.0) ? 0.5 / ggx : 0.0;
}

float Dggx(float NoH,float alphaRoughness)
{
    const float PI = 3.14159265358979;
    float alphaRoughnessSq = alphaRoughness * alphaRoughness;
    float f = (NoH * NoH) * (alphaRoughnessSq - 1.0) + 1.0;
    return alphaRoughnessSq / (PI * f * f);
}

float3 BrdfLambertian(float3 f0,float3 f90,float3 diffuseColor,float VoH)
{
    const float PI = 3.14159265358979;
    return (1.0 - FSchlick(f0, f90, VoH)) * (diffuseColor / PI);
}

#if 0
float3 BrdfSpecularGgx(float3 f0,float3 f90,float alphaRoughness,
float VoH,float NoL,float NoV,float NoH)
{
    float3 F = FSchlick(f0, f90, VoH);
    float Vis = Vggx(NoL, NoV, alphaRoughness);
    float D = Dggx(NoH, alphaRoughness);
    
    return F * Vis * D;
}
#else
float3 IblRadianceGgx(float3 N, float3 V, float roughness, float3 f0, float specularWeight, float intensity)
{
    float NoV = clamp(dot(N, V), 0.0, 1.0);
	
    float2 brdfSamplePoint = clamp(float2(NoV, roughness), 0.0, 1.0);
    float2 fAb = SampleLutGgx(brdfSamplePoint).rg;

    float3 R = normalize(reflect(-V, N));
    float3 specularLight = SampleSpecularPmrem(R, roughness, intensity).rgb;

    // see https://bruop.github.io/ibl/#single_scattering_results at Single Scattering Results
    // Roughness dependent fresnel, from Fdez-Aguera
    float3 fr = max(1.0 - roughness, f0) - f0;
    float3 kS = f0 + fr * pow(1.0 - NoV, 5.0);
    float3 fssEss = kS * fAb.x + fAb.y;

    return specularWeight * specularLight * fssEss;
}
#endif

float3 IblRadianceLambertian(float3 N,float3 V,float roughness,float3 diffuseColor,float3 f0,float specularWeight,float intensity)
{
    float NoV = clamp(dot(N, V), 0.0, 1.0);
    
    float2 brdfSamplePoint = clamp(float2(NoV, roughness), 0.0, 1.0);
    float2 fAb = SampleLutGgx(brdfSamplePoint).rg;

    float3 irradiance = SampleDiffuseIem(N,intensity).rgb;
    
    float3 fr = max(1.0 - roughness, f0) - f0;
    float3 kS = f0 + fr * pow(1.0 - NoV, 5.0);
    float3 fssEss = specularWeight * kS * fAb.x + fAb.y;
    
    float ems = (1.0 - (fAb.x + fAb.y));
    float3 fAvg = specularWeight * (f0 + (1.0 - f0) / 21.0);
    float3 fmsEms = ems * fssEss * fAvg / (1.0 - fAvg * ems);
    float3 kD = diffuseColor * (1.0 - fssEss + fmsEms);
    
    return (fmsEms + kD) * irradiance;
}

float3 IblRadianceGgx(float3 N,float3 V,float roughness,float3 f0)
{
    float NoV = clamp(dot(N, V), 0.0, 1.0);
    
    float2 brdfSamplePoint = clamp(float2(NoV, roughness), 0.0, 1.0);
    float2 fAb = SampleLutGgx(brdfSamplePoint).rg;
    
    float3 R = normalize(reflect(-V, N));
    float3 specularLight = SampleSpecularPmrem(R, roughness).rgb;
    
    float3 fr = max(1.0 - roughness, f0) - f0;
    float3 kS = f0 + fr * pow(1.0 - NoV, 5.0);
    float3 fssEss = kS * fAb.x + fAb.y;
    
    return specularLight * fssEss;
}

#endif
*/