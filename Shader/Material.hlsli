#ifndef __MATERIAL_HLSL__
#define __MATERIAL_HLSL__

#include "Common.hlsli"

#define BASECOLOR_TEXTURE 0
#define METALLIC_ROUGHNESS_TEXTURE 1
#define NORMAL_TEXTURE 2
#define EMISSIVE_TEXTURE 3
#define OCCLUSION_TEXTURE 4

#define DIFFUSE_TEXTURE 5
#define SPECULAR_GLOSSINESS_TEXTURE 6

#define SHEEN_COLOR_TEXTURE 7
#define SHEEN_ROUGHNESS_TEXTURE 8

#define SPECULAR_TEXTURE 9
#define SPECULAR_COLOR_TEXTURE 10

#define CLEARCOAT_TEXTURE 11
#define CLEARCOAT_ROUGHNESS_TEXTURE 12
#define CLEARCOAT_NORMAL_TEXTURE 13

#define TRANSMISSION_TEXTURE 14
#define THICKNESS_TEXTURE 15

Texture2D<float4> materialTextures[16] : register(t1);

// KHR_texture_transform
// https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_texture_transform
struct KhrTextureTransform
{
    float2 offset;
    float rotation;
    float2 scale;
    int texcoord;
};
// https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo
struct TextureInfo
{
    int index; // required.
    int texcoord; // The set index of texture's TEXCOORD attribute used for texture coordinate mapping.
    
    KhrTextureTransform khrTextureTransform;
};
// https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-material-normaltextureinfo
struct NormalTextureInfo
{
    int index; // required
    int texcoord; // The set index of texture's TEXCOORD attribute used for texture coordinate mapping.
    float scale; // scaledNormal = normalize((<sampled normal texture value> * 2.0 - 1.0) * vec3(<normal scale>, <normal scale>, 1.0))
    
    KhrTextureTransform khrTextureTransform;
};
// https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-material-occlusiontextureinfo
struct OcclusionTextureInfo
{
    int index; // required
    int texcoord; // The set index of texture's TEXCOORD attribute used for texture coordinate mapping.
    float strength; // A scalar parameter controlling the amount of occlusion applied. A value of `0.0` means no occlusion. A value of `1.0` means full occlusion. This value affects the final occlusion value as: `1.0 + strength * (<sampled occlusion texture value> - 1.0)`.

    KhrTextureTransform khrTextureTransform;
};
// https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-material-pbrmetallicroughness
struct PbrMetallicRoughness
{
    float4 basecolorFactor; // len = 4. default [1,1,1,1]
    TextureInfo basecolorTexture;
    float metallicFactor; // default 1
    float roughnessFactor; // default 1
    TextureInfo metallicRoughnessTexture;
};
// KHR_materials_pbrSpecularGlossiness
// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Archived/KHR_materials_pbrSpecularGlossiness/README.md
struct KhrMaterialsPbrSpecularGlossiness
{
    float4 diffuseFactor;
    TextureInfo diffuseTexture;
    float3 specularFactor;
    float glossinessFactor;
    TextureInfo specularGlossinessTexture;
};
// KhrMaterialsSheen
// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KhrMaterialsSheen/README.md
struct KhrMaterialsSheen
{
    float3 sheenColorFactor;
    TextureInfo sheenColorTexture;
    float sheenRoughnessFactor;
    TextureInfo sheenRoughnessTexture;
};
// khrMaterialsSpecular
// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/khrMaterialsSpecular/README.md
struct khrMaterialsSpecular
{
    float specularFactor;
    TextureInfo specularTexture;
    float3 specularColorFactor;
    TextureInfo specularColorTexture;
};
// KhrMaterialsClearcoat
// https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KhrMaterialsClearcoat
struct KhrMaterialsClearcoat
{
    float clearcoatFactor;
    TextureInfo clearcoatTexture;
    float clearcoatRoughnessFactor;
    TextureInfo clearcoatRoughnessTexture;
    NormalTextureInfo clearcoatNormalTexture;
};
// KhrMaterialsTransmission
// https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KhrMaterialsTransmission
struct KhrMaterialsTransmission
{
    float transmissionFactor;
    TextureInfo transmissionTexture;
};
// KhrMaterialsVolume
// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KhrMaterialsVolume/README.md
struct KhrMaterialsVolume
{
    float thicknessFactor;
    TextureInfo thicknessTexture;
    float attenuationDistance;
    float3 attenuationColor;
};
// KhrMaterialsEmissiveStrength
// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KhrMaterialsEmissiveStrength/README.md
struct KhrMaterialsEmissiveStrength
{
    float emissiveStrength;
};
// KhrMaterialsIor
// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KhrMaterialsIor/README.md
struct KhrMaterialsIor
{
    float ior;
};



// "OPAQUE" : 0, "MASK" : 1, "BLEND" : 2
#define ALPHAMODE_OPAQUE 0
#define ALPHAMODE_MASK 1
#define ALPHAMODE_BLEND 2
// https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-material
struct MaterialConstants
{
    float3 emissiveFactor; // length 3. default [0, 0, 0]
    int alphaMode; // "OPAQUE" : 0, "MASK" : 1, "BLEND" : 2
    float alphaCutoff; // default 0.5
    bool doubleSided; // default false;

    PbrMetallicRoughness pbrMetallicRoughness;

    NormalTextureInfo normalTexture;
    OcclusionTextureInfo occlusionTexture;
    TextureInfo emissiveTexture;
    
	// Extensions for glTF 2.0
    KhrMaterialsPbrSpecularGlossiness khrMaterialsPbrSpecularGlossiness;
    KhrMaterialsSheen khrMaterialsSheen;
    khrMaterialsSpecular khrMaterialsSpecular;
    KhrMaterialsClearcoat khrMaterialsClearcoat;
    KhrMaterialsTransmission khrMaterialsTransmission;
    KhrMaterialsVolume khrMaterialsVolume;
    KhrMaterialsEmissiveStrength khrMaterialsEmissiveStrength;
    KhrMaterialsIor khrMaterialsIor;
};
StructuredBuffer<MaterialConstants> materials : register(t0);

// KHR_texture_transform
// https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_texture_transform
float2 TransformTexcoord(float2 texcoord, KhrTextureTransform khrTextureTransform)
{
#ifdef khrTextureTransform
	float2 transformed_texcoord = texcoord;
#if 1
	const float2 offset = khrTextureTransform.offset;
	const float rotation = khrTextureTransform.rotation;
	const float2 scale = khrTextureTransform.scale;
        
	const float cosine = cos(rotation);
	const float sine = sin(rotation);
        
	const column_major matrix<float, 3, 3> M = { scale.x * cosine, -scale.y * sine, offset.x, scale.x * sine, scale.y * cosine, offset.x, 0, 0, 1 };
	transformed_texcoord = mul(float3(texcoord.x, texcoord.y, 1), M).xy;
#else
	transformed_texcoord *= khrTextureTransform.scale;
	transformed_texcoord += khrTextureTransform.offset;
#endif
	return transformed_texcoord;
#else
    return texcoord;
#endif
}

struct MaterialInfo
{
    float4 basecolor;
	
    float ior;
    float perceptualRoughness; // roughness value, as authored by the model creator (input to shader)
    float3 f0; // full reflectance color (n incidence angle)

    float alphaRoughness; // roughness mapped to a more linear change in the roughness (proposed by [2])
    float3 cDiff;

    float3 f90; // reflectance color at grazing angle
    float metallic;

    float occlusionFactor;
    float occlusionStrength;
	
    float3 emissiveFactor;
    float emissiveStrength; // KhrMaterialsEmissiveStrength 
	
    float sheenRoughnessFactor;
    float3 sheenColorFactor;

    float3 clearcoatF0;
    float3 clearcoatF90;
    float clearcoatFactor;
    float3 clearcoatNormal;
    float clearcoatRoughnessFactor;

    // khrMaterialsSpecular 
    float specularWeight; // product of specularFactor and specularTexture.a

    float transmissionFactor;

    float thicknessFactor;
    float3 attenuationColor;
    float attenuationDistance;
};

MaterialInfo MakeMaterialInfo(MaterialConstants material, float2 texcoords[2])
{
    MaterialInfo materialInfo;
#if 1
    materialInfo.basecolor = 1.0;
    materialInfo.ior = 1.5;
    materialInfo.perceptualRoughness = 1.0;
    materialInfo.f0 = 0.04;
    materialInfo.alphaRoughness = 1.0;
    materialInfo.cDiff = 1.0;
    materialInfo.f90 = 1.0;
    materialInfo.metallic = 1.0;
    materialInfo.sheenRoughnessFactor = 0.0;
    materialInfo.sheenColorFactor = 0.0;
    materialInfo.clearcoatF0 = 0.04;
    materialInfo.clearcoatF90 = 1.0;
    materialInfo.clearcoatFactor = 0.0;
    materialInfo.clearcoatNormal = 0.0;
    materialInfo.clearcoatRoughnessFactor = 0.0;
    materialInfo.specularWeight = 1.0;
    materialInfo.transmissionFactor = 0.0;
    materialInfo.thicknessFactor = 0.0;
    materialInfo.attenuationColor = 1.0;
    materialInfo.attenuationDistance = 0.0;
#endif
	
#ifdef KHR_materials_pbrSpecularGlossiness
	float4 diffuse_factor = material.KhrMaterialsPbrSpecularGlossiness.diffuse_factor;   
	const int diffuse_texture = material.KhrMaterialsPbrSpecularGlossiness.diffuse_texture.index;
	if (diffuse_texture > -1)
	{
		float2 transformed_texcoord = transform_texcoord(texcoords[material.KhrMaterialsPbrSpecularGlossiness.diffuse_texture.texcoord], material.KhrMaterialsPbrSpecularGlossiness.diffuse_texture.khrTextureTransform);
		float4 sampled = material_textures[DIFFUSE_TEXTURE].Sample(sampler_states[ANISOTROPIC_WRAP], transformed_texcoord);
		sampled.rgb = pow(sampled.rgb, gamma);
		diffuse_factor *= sampled;
	}
	materialInfo.basecolor = diffuse_factor;
#else	
    float4 basecolorFactor = material.pbrMetallicRoughness.basecolorFactor;
    const int basecolorTexture = material.pbrMetallicRoughness.basecolorTexture.index;
    if (basecolorTexture > -1)
    {
        float2 transformedTexcoord = TransformTexcoord(texcoords[material.pbrMetallicRoughness.basecolorTexture.texcoord], material.pbrMetallicRoughness.basecolorTexture.khrTextureTransform);
        float4 sampled = materialTextures[BASECOLOR_TEXTURE].Sample(samplerStates[ANISOTROPIC_WRAP], transformedTexcoord);
        sampled.rgb = pow(sampled.rgb, gamma);
        basecolorFactor *= sampled;
    }
    materialInfo.basecolor = basecolorFactor;
#endif

#ifdef KHR_materials_pbrSpecularGlossiness
	float4 specular_glossiness_factor;
	specular_glossiness_factor.rgb = material.KhrMaterialsPbrSpecularGlossiness.specular_factor;
	specular_glossiness_factor.a = material.KhrMaterialsPbrSpecularGlossiness.glossiness_factor;
	const int specular_glossiness_texture = material.KhrMaterialsPbrSpecularGlossiness.specular_glossiness_texture.index;
	if (specular_glossiness_texture > -1)
	{
		float2 transformed_texcoord = transform_texcoord(texcoords[material.KhrMaterialsPbrSpecularGlossiness.specular_glossiness_texture.texcoord], material.KhrMaterialsPbrSpecularGlossiness.specular_glossiness_texture.khrTextureTransform);
		float4 sampled = material_textures[SPECULAR_GLOSSINESS_TEXTURE].Sample(sampler_states[LINEAR_WRAP], transformed_texcoord);
		specular_glossiness_factor.rgb *= sampled.rgb;
		specular_glossiness_factor.a *= sampled.a;
	}
	materialInfo.f0 = specular_glossiness_factor.rgb;
	materialInfo.perceptualRoughness = specular_glossiness_factor.a;
	materialInfo.perceptualRoughness = 1.0 - materialInfo.perceptualRoughness; // 1 - glossiness
	materialInfo.c_diff = materialInfo.basecolor.rgb * (1.0 - max(max(materialInfo.f0.r, materialInfo.f0.g), materialInfo.f0.b));
#else
    float roughnessFactor = material.pbrMetallicRoughness.roughnessFactor;
    float metallicFactor = material.pbrMetallicRoughness.metallicFactor;
    const int metallicRoughnessTexture = material.pbrMetallicRoughness.metallicRoughnessTexture.index;
    if (metallicRoughnessTexture > -1)
    {
        float2 transformedTexcoord = TransformTexcoord(texcoords[material.pbrMetallicRoughness.metallicRoughnessTexture.texcoord], material.pbrMetallicRoughness.metallicRoughnessTexture.khrTextureTransform);
        float4 sampled = materialTextures[METALLIC_ROUGHNESS_TEXTURE].Sample(samplerStates[LINEAR_WRAP], transformedTexcoord);
        roughnessFactor *= sampled.g;
        metallicFactor *= sampled.b;
    }
    materialInfo.metallic = metallicFactor;
    materialInfo.perceptualRoughness = roughnessFactor;
	
    materialInfo.cDiff = lerp(materialInfo.basecolor.rgb, 0.0, materialInfo.metallic);
#if 1	
	// KhrMaterialsIor
    materialInfo.ior = material.khrMaterialsIor.ior;
    materialInfo.f0 = pow((materialInfo.ior - 1.0) / (materialInfo.ior + 1.0), 2.0);
#else
	materialInfo.f0 = lerp(materialInfo.f0, materialInfo.basecolor.rgb, materialInfo.metallic);
#endif
#endif

#ifdef KhrMaterialsSheen
	float3 sheen_color_factor = material.KhrMaterialsSheen.sheen_color_factor;
	const int sheen_color_texture = material.KhrMaterialsSheen.sheen_color_texture.index;
	if (sheen_color_texture > -1)
	{
		float2 transformed_texcoord = transform_texcoord(texcoords[material.KhrMaterialsSheen.sheen_color_texture.texcoord], material.KhrMaterialsSheen.sheen_color_texture.khrTextureTransform);	
		float4 sampled = material_textures[SHEEN_COLOR_TEXTURE].Sample(sampler_states[LINEAR_WRAP], transformed_texcoord);
		sampled.rgb = pow(sampled.rgb, gamma);
		sheen_color_factor *= sampled.rgb;
	}
	materialInfo.sheen_color_factor = sheen_color_factor;
	
	float sheen_roughness_factor = material.KhrMaterialsSheen.sheen_roughness_factor;
	const int sheen_roughness_texture = material.KhrMaterialsSheen.sheen_roughness_texture.index;
	if (sheen_roughness_texture > -1)
	{
		float2 transformed_texcoord = transform_texcoord(texcoords[material.KhrMaterialsSheen.sheen_roughness_texture.texcoord], material.KhrMaterialsSheen.sheen_roughness_texture.khrTextureTransform);
		float4 sampled = material_textures[SHEEN_ROUGHNESS_TEXTURE].Sample(sampler_states[LINEAR_WRAP], transformed_texcoord);
		sheen_roughness_factor *= sampled.a;
	}
	materialInfo.sheen_roughness_factor = sheen_roughness_factor;
#endif

	
#ifdef khrMaterialsSpecular
	float3 specular_color_factor = material.khrMaterialsSpecular.specular_color_factor;
	const int specular_color_texture = material.khrMaterialsSpecular.specular_color_texture.index;
	if (specular_color_texture > -1)
	{
		float2 transformed_texcoord = transform_texcoord(texcoords[material.khrMaterialsSpecular.specular_color_texture.texcoord], material.khrMaterialsSpecular.specular_color_texture.khrTextureTransform);
		float4 sampled = material_textures[SPECULAR_COLOR_TEXTURE].Sample(sampler_states[LINEAR_WRAP], transformed_texcoord);
		specular_color_factor *= sampled.rgb;
	}
	
	float specular_factor = material.khrMaterialsSpecular.specular_factor;
	const int specular_texture = material.khrMaterialsSpecular.specular_texture.index;
	if (specular_texture > -1)
	{
		float2 transformed_texcoord = transform_texcoord(texcoords[material.khrMaterialsSpecular.specular_texture.texcoord], material.khrMaterialsSpecular.specular_texture.khrTextureTransform);
		float4 sampled = material_textures[SPECULAR_TEXTURE].Sample(sampler_states[LINEAR_WRAP], transformed_texcoord);
		specular_factor *= sampled.a; // A texture that defines the strength of the specular reflection, stored in the alpha (A) channel. This will be multiplied by specularFactor.
	}
	float3 dielectric_specular_f0 = min(materialInfo.f0 * specular_color_factor, 1.0);
	materialInfo.f0 = lerp(dielectric_specular_f0, materialInfo.basecolor.rgb, materialInfo.metallic);
	materialInfo.specular_weight = specular_factor;
	materialInfo.c_diff = lerp(materialInfo.basecolor.rgb, 0, materialInfo.metallic);
#endif
	
#ifdef KhrMaterialsClearcoat
	float clearcoat_factor = material.KhrMaterialsClearcoat.clearcoat_factor;
	const int clearcoat_texture = material.KhrMaterialsClearcoat.clearcoat_texture.index;
	if (clearcoat_texture > -1)
	{
		float2 transformed_texcoord = transform_texcoord(texcoords[material.KhrMaterialsClearcoat.clearcoat_texture.texcoord], material.KhrMaterialsClearcoat.clearcoat_texture.khrTextureTransform);
		float4 sampled = material_textures[CLEARCOAT_TEXTURE].Sample(sampler_states[LINEAR_WRAP], transformed_texcoord);
		clearcoat_factor *= sampled.r;
	}
	materialInfo.clearcoat_factor = clearcoat_factor;
	
	float clearcoat_roughness_factor = material.KhrMaterialsClearcoat.clearcoat_roughness_factor;
	const int clearcoat_roughness_texture = material.KhrMaterialsClearcoat.clearcoat_roughness_texture.index;
	if (clearcoat_roughness_texture > -1)
	{
		float2 transformed_texcoord = transform_texcoord(texcoords[material.KhrMaterialsClearcoat.clearcoat_roughness_texture.texcoord], material.KhrMaterialsClearcoat.clearcoat_roughness_texture.khrTextureTransform);
		float4 sampled = material_textures[CLEARCOAT_ROUGHNESS_TEXTURE].Sample(sampler_states[LINEAR_WRAP], transformed_texcoord);
		clearcoat_roughness_factor *= sampled.g;
	}
	materialInfo.clearcoat_roughness_factor = clamp(clearcoat_roughness_factor, 0.0, 1.0);

	materialInfo.clearcoat_f0 = pow((materialInfo.ior - 1.0) / (materialInfo.ior + 1.0), 2.0);
	materialInfo.clearcoat_f90 = 1.0;
#endif
	
#ifdef KhrMaterialsTransmission
	float transmission_factor = material.KhrMaterialsTransmission.transmission_factor;
	const int transmission_texture = material.KhrMaterialsTransmission.transmission_texture.index;
	if (transmission_texture > -1)
	{
		float2 transformed_texcoord = transform_texcoord(texcoords[material.KhrMaterialsTransmission.transmission_texture.texcoord], material.KhrMaterialsTransmission.transmission_texture.khrTextureTransform);
		float4 sampled = material_textures[TRANSMISSION_TEXTURE].Sample(sampler_states[LINEAR_WRAP], transformed_texcoord);
		transmission_factor *= sampled.g;
	}
	materialInfo.transmission_factor = transmission_factor;
#endif
	
#ifdef KhrMaterialsVolume
	float thickness_factor = material.KhrMaterialsVolume.thickness_factor;
	const int thickness_texture = material.KhrMaterialsVolume.thickness_texture.index;
	if (thickness_texture > -1)
	{
		float2 transformed_texcoord = transform_texcoord(texcoords[material.KhrMaterialsVolume.thickness_texture.texcoord], material.KhrMaterialsVolume.thickness_texture.khrTextureTransform);
		float4 sampled = material_textures[THICKNESS_TEXTURE].Sample(sampler_states[LINEAR_WRAP], transformed_texcoord);
		thickness_factor *= sampled.g;
	}
	materialInfo.thickness_factor = thickness_factor;
	materialInfo.attenuation_color = material.KhrMaterialsVolume.attenuation_color;
	materialInfo.attenuation_distance = material.KhrMaterialsVolume.attenuation_distance;
#endif
	
    float occlusionFactor = 1.0;
    const int occlusionTexture = material.occlusionTexture.index;
    if (occlusionTexture > -1)
    {
        float2 transformedTexcoord = TransformTexcoord(texcoords[material.occlusionTexture.texcoord], material.occlusionTexture.khrTextureTransform);
        float4 sampled = materialTextures[OCCLUSION_TEXTURE].Sample(samplerStates[LINEAR_WRAP], transformedTexcoord);
        occlusionFactor *= sampled.r;
    }
    materialInfo.occlusionFactor = occlusionFactor;
    materialInfo.occlusionStrength = material.occlusionTexture.strength;

    float3 emissiveFactor = material.emissiveFactor;
    const int emissiveTexture = material.emissiveTexture.index;
    if (emissiveTexture > -1)
    {
        float2 transformedTexcoord = TransformTexcoord(texcoords[material.emissiveTexture.texcoord], material.emissiveTexture.khrTextureTransform);
        float4 sampled = materialTextures[EMISSIVE_TEXTURE].Sample(samplerStates[ANISOTROPIC_WRAP], transformedTexcoord);
        sampled.rgb = pow(sampled.rgb, gamma);
        emissiveFactor *= sampled.rgb;
    }
#if 0
	else
	{
		emissive_factor *= materialInfo.basecolor.rgb;
	}
#endif
    materialInfo.emissiveFactor = emissiveFactor;
    materialInfo.emissiveStrength = material.khrMaterialsEmissiveStrength.emissiveStrength;
	
	
#if 0	
	materialInfo.perceptualRoughness = clamp(materialInfo.perceptualRoughness, 0.0, 1.0);
#else
    materialInfo.perceptualRoughness = clamp(materialInfo.perceptualRoughness, 0.045, 1.0);
#endif
    materialInfo.metallic = clamp(materialInfo.metallic, 0.0, 1.0);
	
    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness.
    materialInfo.alphaRoughness = materialInfo.perceptualRoughness * materialInfo.perceptualRoughness;

	// Compute reflectance.
    float reflectance = max(max(materialInfo.f0.r, materialInfo.f0.g), materialInfo.f0.b);
	
	// Anything less than 2% is physically impossible and is instead considered to be shadowing. Compare to "Real-Time-Rendering" 4th editon on page 325.
    materialInfo.f90 = 1.0;
	
	
    return materialInfo;
	
}
#endif // __MATERIAL_HLSL__