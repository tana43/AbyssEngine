#include "GeometricSubstance.h"

#include <iostream>
#include <map>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <sstream>

#include <functional>
#include <filesystem>
#include <tuple>

#define TINYGLTF_IMPLEMENTATION
#include "tinygltf-release/tiny_gltf.h"

#include "Misc.h"
#include "Shader.h"
#include "Texture.h"

using namespace AbyssEngine;

void GeometricSubstance::ExtractAssets(const tinygltf::Model& transmissionModel)
{
	asset_.version_ = transmissionModel.asset.version;
	asset_.generator_ = transmissionModel.asset.generator;
	asset_.minversion_ = transmissionModel.asset.minVersion;
	asset_.copyright_ = transmissionModel.asset.copyright;
}
void GeometricSubstance::ExtractScenes(const tinygltf::Model& transmissionModel)
{
	for (decltype(transmissionModel.scenes)::const_reference transmission_scene : transmissionModel.scenes)
	{
		Scene& Scene = scenes_.emplace_back();
		Scene.name_ = transmission_scene.name;
		Scene.nodes_ = transmission_scene.nodes;
	}
}
void GeometricSubstance::ExtractNodes(const tinygltf::Model& transmissionModel)
{
	for (decltype(transmissionModel.nodes)::const_reference transmission_node : transmissionModel.nodes)
	{
		Node& Node = nodes_.emplace_back();
		Node.name_ = transmission_node.name;
		Node.camera_ = transmission_node.camera;
		Node.skin_ = transmission_node.skin;
		Node.mesh_ = transmission_node.mesh;
		Node.children_ = transmission_node.children;

		if (!transmission_node.matrix.empty())
		{
			DirectX::XMFLOAT4X4 matrix;
			for (size_t row = 0; row < 4; row++)
			{
				for (size_t column = 0; column < 4; column++)
				{
					matrix(row, column) = static_cast<float>(transmission_node.matrix.at(4 * row + column));
				}
			}

			DirectX::XMVECTOR S, T, R;
			bool succeed = DirectX::XMMatrixDecompose(&S, &R, &T, DirectX::XMLoadFloat4x4(&matrix));
			_ASSERT_EXPR(succeed, L"Failed to decompose matrix.");

			DirectX::XMStoreFloat3(&Node.scale_, S);
			DirectX::XMStoreFloat4(&Node.rotation_, R);
			DirectX::XMStoreFloat3(&Node.translation_, T);
		}
		else
		{
			if (transmission_node.scale.size() > 0)
			{
				Node.scale_.x = static_cast<float>(transmission_node.scale.at(0));
				Node.scale_.y = static_cast<float>(transmission_node.scale.at(1));
				Node.scale_.z = static_cast<float>(transmission_node.scale.at(2));
			}
			if (transmission_node.translation.size() > 0)
			{
				Node.translation_.x = static_cast<float>(transmission_node.translation.at(0));
				Node.translation_.y = static_cast<float>(transmission_node.translation.at(1));
				Node.translation_.z = static_cast<float>(transmission_node.translation.at(2));
			}
			if (transmission_node.rotation.size() > 0)
			{
				Node.rotation_.x = static_cast<float>(transmission_node.rotation.at(0));
				Node.rotation_.y = static_cast<float>(transmission_node.rotation.at(1));
				Node.rotation_.z = static_cast<float>(transmission_node.rotation.at(2));
				Node.rotation_.w = static_cast<float>(transmission_node.rotation.at(3));
			}
		}
		Node.weights_ = transmission_node.weights;
	}
	for (size_t scene_index{}; scene_index < scenes_.size(); ++scene_index)
	{
		CumulateTransforms(nodes_, scene_index);
	}
}
void GeometricSubstance::ExtractExtensions(const tinygltf::Model& transmissionModel)
{
	// Check extension materials.
	extensionsUsed_ = transmissionModel.extensionsUsed;
	extensionsRequired_ = transmissionModel.extensionsRequired;
#if _DEBUG
	for (std::vector<std::string>::const_reference extensions_used : transmissionModel.extensionsUsed)
	{
		OutputDebugStringA((extensions_used + "\n").c_str());
	}
	for (std::vector<std::string>::const_reference extensions_required : transmissionModel.extensionsRequired)
	{
		OutputDebugStringA((extensions_required + "\n").c_str());
	}
#endif

	tinygltf::ExtensionMap::const_iterator extension;

	// KHR_lights_punctual
	// https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_lights_punctual
	extension = transmissionModel.extensions.find("KHR_lights_punctual");
	if (extension != transmissionModel.extensions.end())
	{
		if (extension->second.Has("lights"))
		{
			for (tinygltf::Value::Array::const_reference light : extension->second.Get<tinygltf::Value::Object>().find("lights")->second.Get<tinygltf::Value::Array>())
			{
				punctual_light punctual_light;
				if (light.Has("name"))
				{
					punctual_light.type_ = light.Get<tinygltf::Value::Object>().find("name")->second.Get<std::string>();
				}
				if (light.Has("color"))
				{
					std::vector<tinygltf::Value::Array>::const_reference color_ = light.Get<tinygltf::Value::Object>().find("color")->second.Get<tinygltf::Value::Array>();
					punctual_light.color_[0] = static_cast<float>(color_.at(0).GetNumberAsDouble());
					punctual_light.color_[1] = static_cast<float>(color_.at(1).GetNumberAsDouble());
					punctual_light.color_[2] = static_cast<float>(color_.at(2).GetNumberAsDouble());
				}
				if (light.Has("intensity"))
				{
					punctual_light.intensity_ = static_cast<float>(light.Get<tinygltf::Value::Object>().find("intensity")->second.GetNumberAsDouble());
				}
				if (light.Has("type"))
				{
					punctual_light.type_ = light.Get<tinygltf::Value::Object>().find("type")->second.Get<std::string>();
				}
				if (light.Has("range"))
				{
					punctual_light.intensity_ = static_cast<float>(light.Get<tinygltf::Value::Object>().find("range")->second.GetNumberAsDouble());
				}
				if (light.Has("spot"))
				{
					if (light.Get<tinygltf::Value::Object>().find("spot")->second.Has("innerConeAngle"))
					{
						punctual_light.innerConeAngle_ = static_cast<float>(light.Get<tinygltf::Value::Object>().find("spot")->second.Get<tinygltf::Value::Object>().find("innerConeAngle")->second.GetNumberAsDouble());
					}
					if (light.Get<tinygltf::Value::Object>().find("spot")->second.Has("outerConeAngle"))
					{
						punctual_light.outerConeAngle_ = static_cast<float>(light.Get<tinygltf::Value::Object>().find("spot")->second.Get<tinygltf::Value::Object>().find("outerConeAngle")->second.GetNumberAsDouble());
					}
				}
				punctualLights_.emplace_back(std::move(punctual_light));
			}
		}
	}

	// KHR_materials_variants
	// https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_variants
	if (extension != transmissionModel.extensions.end())
	{
		if (extension->second.Has("variants"))
		{
			for (tinygltf::Value::Array::const_reference variant : extension->second.Get<tinygltf::Value::Object>().find("variants")->second.Get<tinygltf::Value::Array>())
			{
				variants_.emplace_back(variant.Get<tinygltf::Value::Object>().find("name")->second.Get<std::string>());
			}
		}
	}
}

void GeometricSubstance::ExtractMaterials(const tinygltf::Model& transmissionModel)
{
	ID3D11Device* device = DXSystem::device_.Get();

	for (std::vector<tinygltf::Material>::const_reference transmissionMaterial : transmissionModel.materials)
	{
		std::vector<Material>::reference material_ = materials_.emplace_back();

		material_.name_ = transmissionMaterial.name;

		material_.data_.emissiveFactor_[0] = static_cast<float>(transmissionMaterial.emissiveFactor.at(0));
		material_.data_.emissiveFactor_[1] = static_cast<float>(transmissionMaterial.emissiveFactor.at(1));
		material_.data_.emissiveFactor_[2] = static_cast<float>(transmissionMaterial.emissiveFactor.at(2));

		material_.data_.alphaMode_ = transmissionMaterial.alphaMode == "OPAQUE" ? 0 : transmissionMaterial.alphaMode == "MASK" ? 1 : transmissionMaterial.alphaMode == "BLEND" ? 2 : 0;
		material_.data_.alphaCutoff_ = static_cast<float>(transmissionMaterial.alphaCutoff);
		material_.data_.doubleSided_ = transmissionMaterial.doubleSided ? 1 : 0;

		material_.data_.pbrMetallicRoughness_.basecolorFactor_[0] = static_cast<float>(transmissionMaterial.pbrMetallicRoughness.baseColorFactor.at(0));
		material_.data_.pbrMetallicRoughness_.basecolorFactor_[1] = static_cast<float>(transmissionMaterial.pbrMetallicRoughness.baseColorFactor.at(1));
		material_.data_.pbrMetallicRoughness_.basecolorFactor_[2] = static_cast<float>(transmissionMaterial.pbrMetallicRoughness.baseColorFactor.at(2));
		material_.data_.pbrMetallicRoughness_.basecolorFactor_[3] = static_cast<float>(transmissionMaterial.pbrMetallicRoughness.baseColorFactor.at(3));
		material_.data_.pbrMetallicRoughness_.basecolorTexture_.index_ = transmissionMaterial.pbrMetallicRoughness.baseColorTexture.index;
		material_.data_.pbrMetallicRoughness_.basecolorTexture_.texcoord_ = transmissionMaterial.pbrMetallicRoughness.baseColorTexture.texCoord;
		material_.data_.pbrMetallicRoughness_.basecolorTexture_.khrTextureTransform_.init(transmissionMaterial.pbrMetallicRoughness.baseColorTexture.extensions);
		material_.data_.pbrMetallicRoughness_.metallicFactor_ = static_cast<float>(transmissionMaterial.pbrMetallicRoughness.metallicFactor);
		material_.data_.pbrMetallicRoughness_.roughnessFactor_ = static_cast<float>(transmissionMaterial.pbrMetallicRoughness.roughnessFactor);
		material_.data_.pbrMetallicRoughness_.metallicRoughnessTexture_.index_ = transmissionMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
		material_.data_.pbrMetallicRoughness_.metallicRoughnessTexture_.texcoord_ = transmissionMaterial.pbrMetallicRoughness.metallicRoughnessTexture.texCoord;
		material_.data_.pbrMetallicRoughness_.metallicRoughnessTexture_.khrTextureTransform_.init(transmissionMaterial.pbrMetallicRoughness.metallicRoughnessTexture.extensions);

		material_.data_.normalTexture_.index_ = transmissionMaterial.normalTexture.index;
		material_.data_.normalTexture_.texcoord_ = transmissionMaterial.normalTexture.texCoord;
		material_.data_.normalTexture_.scale_ = static_cast<float>(transmissionMaterial.normalTexture.scale);
		material_.data_.normalTexture_.khrTextureTransform_.init(transmissionMaterial.normalTexture.extensions);

		material_.data_.occlusionTexture_.index_ = transmissionMaterial.occlusionTexture.index;
		material_.data_.occlusionTexture_.texcoord_ = transmissionMaterial.occlusionTexture.texCoord;
		material_.data_.occlusionTexture_.strength = static_cast<float>(transmissionMaterial.occlusionTexture.strength);
		material_.data_.occlusionTexture_.khr_texture_transform.init(transmissionMaterial.occlusionTexture.extensions);

		material_.data_.emissiveTexture_.index_ = transmissionMaterial.emissiveTexture.index;
		material_.data_.emissiveTexture_.texcoord_ = transmissionMaterial.emissiveTexture.texCoord;
		material_.data_.emissiveTexture_.khrTextureTransform_.init(transmissionMaterial.emissiveTexture.extensions);

		for (tinygltf::ExtensionMap::const_reference extension : transmissionMaterial.extensions)
		{
			// KHR_materials_pbrSpecularGlossiness
			// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Archived/KHR_materials_pbrSpecularGlossiness/README.md
			if (extension.first == "KHR_materials_pbrSpecularGlossiness")
			{
				if (extension.second.Has("diffuseFactor"))
				{
					std::vector<tinygltf::Value>::const_reference diffuse_factor = extension.second.Get("diffuseFactor");
					material_.data_.KhrMaterialsPbrSpecularGlossiness_.diffuseFactor_[0] = static_cast<float>(diffuse_factor.Get<tinygltf::Value::Array>().at(0).Get<double>());
					material_.data_.KhrMaterialsPbrSpecularGlossiness_.diffuseFactor_[1] = static_cast<float>(diffuse_factor.Get<tinygltf::Value::Array>().at(1).Get<double>());
					material_.data_.KhrMaterialsPbrSpecularGlossiness_.diffuseFactor_[2] = static_cast<float>(diffuse_factor.Get<tinygltf::Value::Array>().at(2).Get<double>());
					material_.data_.KhrMaterialsPbrSpecularGlossiness_.diffuseFactor_[3] = static_cast<float>(diffuse_factor.Get<tinygltf::Value::Array>().at(3).Get<double>());
				}
				if (extension.second.Has("diffuseTexture"))
				{
					std::vector<tinygltf::Value>::const_reference diffuse_texture = extension.second.Get("diffuseTexture");
					material_.data_.KhrMaterialsPbrSpecularGlossiness_.diffuseTexture_.init(diffuse_texture.Get<tinygltf::Value::Object>());
				}
				if (extension.second.Has("specularFactor"))
				{
					std::vector<tinygltf::Value>::const_reference specular_factor = extension.second.Get("specularFactor");
					material_.data_.KhrMaterialsPbrSpecularGlossiness_.specularFactor_[0] = static_cast<float>(specular_factor.Get<tinygltf::Value::Array>().at(0).Get<double>());
					material_.data_.KhrMaterialsPbrSpecularGlossiness_.specularFactor_[1] = static_cast<float>(specular_factor.Get<tinygltf::Value::Array>().at(1).Get<double>());
					material_.data_.KhrMaterialsPbrSpecularGlossiness_.specularFactor_[2] = static_cast<float>(specular_factor.Get<tinygltf::Value::Array>().at(2).Get<double>());
				}
				if (extension.second.Has("glossinessFactor"))
				{
					std::vector<tinygltf::Value>::const_reference glossiness_factor = extension.second.Get("glossinessFactor");
					material_.data_.KhrMaterialsPbrSpecularGlossiness_.glossinessFactor_ = static_cast<float>(glossiness_factor.GetNumberAsDouble());
				}
				if (extension.second.Has("specularGlossinessTexture"))
				{
					std::vector<tinygltf::Value>::const_reference specular_glossiness_texture = extension.second.Get("specularGlossinessTexture");
					material_.data_.KhrMaterialsPbrSpecularGlossiness_.specularGlossinessTexture_.init(specular_glossiness_texture.Get<tinygltf::Value::Object>());
				}
			}
			// KHR_materials_sheen
			// https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Khronos/KHR_materials_sheen/README.md
			if (extension.first == "KHR_materials_sheen")
			{
				if (extension.second.Has("sheenColorFactor"))
				{
					std::vector<tinygltf::Value>::const_reference sheen_color_factor = extension.second.Get("sheenColorFactor");
					material_.data_.khrMaterialsSheen_.sheenColorFactor_[0] = static_cast<float>(sheen_color_factor.Get<tinygltf::Value::Array>().at(0).Get<double>());
					material_.data_.khrMaterialsSheen_.sheenColorFactor_[1] = static_cast<float>(sheen_color_factor.Get<tinygltf::Value::Array>().at(1).Get<double>());
					material_.data_.khrMaterialsSheen_.sheenColorFactor_[2] = static_cast<float>(sheen_color_factor.Get<tinygltf::Value::Array>().at(2).Get<double>());
				}
				if (extension.second.Has("sheenColorTexture"))
				{
					std::vector<tinygltf::Value>::const_reference sheen_color_texture = extension.second.Get("sheenColorTexture");
					material_.data_.khrMaterialsSheen_.sheenColorTexture_.init(sheen_color_texture.Get<tinygltf::Value::Object>());
				}
				if (extension.second.Has("sheenRoughnessFactor"))
				{
					std::vector<tinygltf::Value>::const_reference sheen_roughness_factor = extension.second.Get("sheenRoughnessFactor");
					material_.data_.khrMaterialsSheen_.sheenRoughnessFactor_ = static_cast<float>(sheen_roughness_factor.GetNumberAsDouble());
				}

				if (extension.second.Has("sheenRoughnessTexture"))
				{
					std::vector<tinygltf::Value>::const_reference sheen_roughness_texture = extension.second.Get("sheenRoughnessTexture");
					material_.data_.khrMaterialsSheen_.sheenRoughnessTexture_.init(sheen_roughness_texture.Get<tinygltf::Value::Object>());
				}
			}
			// KHR_materials_specular
			// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_specular/README.md
			if (extension.first == "KHR_materials_specular")
			{
				if (extension.second.Has("specularFactor"))
				{
					std::vector<tinygltf::Value>::const_reference specular_factor = extension.second.Get("specularFactor");
					material_.data_.khrMaterialsSpecular_.specularFactor_ = static_cast<float>(specular_factor.GetNumberAsDouble());
				}
				if (extension.second.Has("specularTexture"))
				{
					std::vector<tinygltf::Value>::const_reference specular_texture = extension.second.Get("specularTexture");
					material_.data_.khrMaterialsSpecular_.specularTexture_.init(specular_texture.Get<tinygltf::Value::Object>());
				}
				if (extension.second.Has("specularColorFactor"))
				{
					std::vector<tinygltf::Value>::const_reference specular_factor = extension.second.Get("specularColorFactor");
					material_.data_.khrMaterialsSpecular_.specularColorFactor_[0] = static_cast<float>(specular_factor.Get<tinygltf::Value::Array>().at(0).Get<double>());
					material_.data_.khrMaterialsSpecular_.specularColorFactor_[1] = static_cast<float>(specular_factor.Get<tinygltf::Value::Array>().at(1).Get<double>());
					material_.data_.khrMaterialsSpecular_.specularColorFactor_[2] = static_cast<float>(specular_factor.Get<tinygltf::Value::Array>().at(2).Get<double>());
				}
				if (extension.second.Has("specularColorTexture"))
				{
					std::vector<tinygltf::Value>::const_reference specular_color_texture = extension.second.Get("specularColorTexture");
					material_.data_.khrMaterialsSpecular_.specularColorTexture_.init(specular_color_texture.Get<tinygltf::Value::Object>());
				}
			}
			// KHR_materials_clearcoat
			// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_specular/README.md
			if (extension.first == "KHR_materials_clearcoat")
			{
				if (extension.second.Has("clearcoatFactor"))
				{
					std::vector<tinygltf::Value>::const_reference clearcoat_factor = extension.second.Get("clearcoatFactor");
					material_.data_.khrMaterialsClearcoat_.clearcoatFactor_ = static_cast<float>(clearcoat_factor.GetNumberAsDouble());
				}
				if (extension.second.Has("clearcoatTexture"))
				{
					std::vector<tinygltf::Value>::const_reference clearcoat_texture = extension.second.Get("clearcoatTexture");
					material_.data_.khrMaterialsClearcoat_.clearcoatTexture_.init(clearcoat_texture.Get<tinygltf::Value::Object>());
				}
				if (extension.second.Has("clearcoatRoughnessFactor"))
				{
					std::vector<tinygltf::Value>::const_reference clearcoat_roughness_factor = extension.second.Get("clearcoatRoughnessFactor");
					material_.data_.khrMaterialsClearcoat_.clearcoatRoughnessFactor_ = static_cast<float>(clearcoat_roughness_factor.GetNumberAsDouble());
				}
				if (extension.second.Has("clearcoatRoughnessTexture"))
				{
					std::vector<tinygltf::Value>::const_reference clearcoat_roughness_texture = extension.second.Get("clearcoatRoughnessTexture");
					material_.data_.khrMaterialsClearcoat_.clearcoatRoughnessTexture_.init(clearcoat_roughness_texture.Get<tinygltf::Value::Object>());
				}
				if (extension.second.Has("clearcoatNormalTexture"))
				{
					std::vector<tinygltf::Value>::const_reference clearcoat_normal_texture = extension.second.Get("clearcoatNormalTexture");
					material_.data_.khrMaterialsClearcoat_.clearcoatNormalTexture_.init(clearcoat_normal_texture.Get<tinygltf::Value::Object>());
				}
			}
			// KHR_materials_transmission
			// https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_transmission
			if (extension.first == "KHR_materials_transmission")
			{
				if (extension.second.Has("transmissionFactor"))
				{
					std::vector<tinygltf::Value>::const_reference transmission_factor = extension.second.Get("transmissionFactor");
					material_.data_.khrMaterialsTransmission_.transmissionFactor_ = static_cast<float>(transmission_factor.GetNumberAsDouble());
				}
				if (extension.second.Has("transmissionTexture"))
				{
					std::vector<tinygltf::Value>::const_reference transmission_texture = extension.second.Get("transmissionTexture");
					material_.data_.khrMaterialsTransmission_.transmissionTexture_.init(transmission_texture.Get<tinygltf::Value::Object>());
				}
			}
			// KHR_materials_volume
			// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_volume/README.md
			if (extension.first == "KHR_materials_volume")
			{
				if (extension.second.Has("thicknessFactor"))
				{
					std::vector<tinygltf::Value>::const_reference thickness_factor = extension.second.Get("thicknessFactor");
					material_.data_.khrMaterialsVolume_.thicknessFactor_ = static_cast<float>(thickness_factor.GetNumberAsDouble());
				}
				if (extension.second.Has("thicknessTexture"))
				{
					std::vector<tinygltf::Value>::const_reference thickness_texture = extension.second.Get("thicknessTexture");
					material_.data_.khrMaterialsVolume_.thicknessTexture_.init(thickness_texture.Get<tinygltf::Value::Object>());
				}
				if (extension.second.Has("attenuationDistance"))
				{
					std::vector<tinygltf::Value>::const_reference attenuation_distance = extension.second.Get("attenuationDistance");
					material_.data_.khrMaterialsVolume_.attenuationDistance_ = static_cast<float>(attenuation_distance.GetNumberAsDouble());
				}
				if (extension.second.Has("attenuationColor"))
				{
					std::vector<tinygltf::Value>::const_reference attenuation_color = extension.second.Get("attenuationColor");
					material_.data_.khrMaterialsVolume_.attenuationColor_[0] = static_cast<float>(attenuation_color.Get<tinygltf::Value::Array>().at(0).Get<double>());
					material_.data_.khrMaterialsVolume_.attenuationColor_[1] = static_cast<float>(attenuation_color.Get<tinygltf::Value::Array>().at(1).Get<double>());
					material_.data_.khrMaterialsVolume_.attenuationColor_[2] = static_cast<float>(attenuation_color.Get<tinygltf::Value::Array>().at(2).Get<double>());
				}
			}
			// KHR_materials_emissive_strength
			// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_emissive_strength/README.md
			if (extension.first == "KHR_materials_emissive_strength")
			{
				if (extension.second.Has("emissiveStrength"))
				{
					std::vector<tinygltf::Value>::const_reference emissive_strength = extension.second.Get("emissiveStrength");
					material_.data_.khrMaterialsEmissiveStrength_.emissiveStrength_ = static_cast<float>(emissive_strength.GetNumberAsDouble());
				}
			}
			// KHR_materials_ior
			// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_ior/README.md
			if (extension.first == "KHR_materials_ior")
			{
				if (extension.second.Has("ior"))
				{
					std::vector<tinygltf::Value>::const_reference ior = extension.second.Get("ior");
					material_.data_.khrMaterialsIor_.ior_ = static_cast<float>(ior.GetNumberAsDouble());
				}
			}
			// KHR_materials_unlit
			if (extension.first == "KHR_materials_unlit")
			{
			}
		}
	}

	// Add a dummy material at the end.
	materials_.emplace_back();

	// Create material data as shader resource view on GPU
	HRESULT hr;
	std::vector<Material::CBuffer> materialData;
	for (std::vector<Material>::const_reference material_ : materials_)
	{
		materialData.emplace_back(material_.data_);
	}
	Microsoft::WRL::ComPtr<ID3D11Buffer> materialBuffer;
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Material::CBuffer) * materialData.size());
	bufferDesc.StructureByteStride = sizeof(Material::CBuffer);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = materialData.data();
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&bufferDesc, &subresource_data, materialBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
	shader_resource_view_desc.Format = DXGI_FORMAT_UNKNOWN;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	shader_resource_view_desc.Buffer.ElementOffset = 0;
	shader_resource_view_desc.Buffer.NumElements = static_cast<UINT>(materialData.size());
	hr = device->CreateShaderResourceView(materialBuffer.Get(), &shader_resource_view_desc, materialResourceView_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

}
void GeometricSubstance::ExtractTextures(const tinygltf::Model& transmissionModel)
{
	HRESULT hr = S_OK;

	for (decltype(transmissionModel.textures)::const_reference transmission_texture : transmissionModel.textures)
	{
		GSTexture& texture = textures_.emplace_back();
		texture.name_ = transmission_texture.name;
		texture.sampler_ = transmission_texture.sampler;
		texture.source_ = transmission_texture.source;
	}
	for (decltype(transmissionModel.samplers)::const_reference transmissionSampler : transmissionModel.samplers)
	{
		Sampler& sampler = samplers_.emplace_back();
		sampler.magFilter_ = transmissionSampler.magFilter;
		sampler.minFilter_ = transmissionSampler.minFilter;
		sampler.wrapS_ = transmissionSampler.wrapS;
		sampler.wrapT_ = transmissionSampler.wrapT;
		sampler.name_ = transmissionSampler.name;
	}
	for (decltype(transmissionModel.images)::const_reference transmission_image : transmissionModel.images)
	{
		Image& image = images_.emplace_back();
		image.name_ = transmission_image.name;
		image.width_ = transmission_image.width;
		image.height_ = transmission_image.height;
		image.component_ = transmission_image.component;
		image.bits_ = transmission_image.bits;
		image.pixelType_ = transmission_image.pixel_type;
		image.bufferView_ = transmission_image.bufferView;
		image.mimeType_ = transmission_image.mimeType;
		image.uri_ = transmission_image.uri;
		image.asIs_ = transmission_image.as_is;
	}

	for (decltype(images_)::const_reference image : images_)
	{
		if (image.bufferView_ > -1)
		{
			const tinygltf::BufferView& bufferView = transmissionModel.bufferViews.at(image.bufferView_);
			const tinygltf::Buffer& buffer = transmissionModel.buffers.at(bufferView.buffer);
			const byte* data = buffer.data.data() + bufferView.byteOffset;

			std::wstring name_;
			if (image.name_.size() > 0)
			{
				name_ = std::wstring(name_.begin(), name_.end()) + L":" + std::wstring(image.name_.begin(), image.name_.end());
			}
			else
			{
				std::wstringstream wss;
				wss << std::wstring(name_.begin(), name_.end()) << ":" << image.bufferView_ << L":" << bufferView.buffer << L":" << bufferView.byteOffset;
				name_ = wss.str();
			}

			ID3D11ShaderResourceView* texture_resource_view = {};
			hr = Texture::LoadTextureFromMemory(data, bufferView.byteLength, &texture_resource_view);
			if (hr == S_OK)
			{
				textureResourceViews_.emplace_back().Attach(texture_resource_view);
			}
		}
		else if (image.uri_.size() > 0)
		{
			const std::filesystem::path path(name_);
			ID3D11ShaderResourceView* shader_resource_view{};
			hr = Texture::LoadTextureFromFile((path.parent_path().concat("/").string() + std::string(image.uri_.begin(), image.uri_.end())).c_str(), &shader_resource_view,nullptr);
			if (hr == S_OK)
			{
				textureResourceViews_.emplace_back().Attach(shader_resource_view);
			}
		}
		else
		{
			_ASSERT_EXPR(FALSE, L"Not support this kind of transmission_image.");
		}
	}
}
void GeometricSubstance::AppendAnimations(const std::vector<std::string>& filenames)
{
	for (std::vector<std::string>::const_reference filename : filenames)
	{
		AppendAnimation(filename);
	}
}
void GeometricSubstance::AppendAnimation(const std::string& filename)
{
	tinygltf::TinyGLTF tinyGltf;
#if 1
	tinyGltf.SetImageLoader(NullLoadImageData, nullptr);
#endif

	tinygltf::Model transmissionModel;
	std::string error, warning;
	bool succeeded = false;
	if (filename.find(".glb") != std::string::npos)
	{
		succeeded = tinyGltf.LoadBinaryFromFile(&transmissionModel, &error, &warning, filename.c_str());
	}
	else if (filename.find(".gltf") != std::string::npos)
	{
		succeeded = tinyGltf.LoadASCIIFromFile(&transmissionModel, &error, &warning, filename.c_str());
	}
	if (!warning.empty())
	{
		OutputDebugStringA(warning.c_str());
	}
	if (!error.empty())
	{
		throw std::exception(error.c_str());
	}
	if (!succeeded)
	{
		throw std::exception("Failed to load glTF file");
	}
	ExtractAnimations(transmissionModel);
}
void GeometricSubstance::ExtractAnimations(const tinygltf::Model& transmissionModel)
{
	for (std::vector<tinygltf::Skin>::const_reference transmissionSkin : transmissionModel.skins)
	{
		Skin& skin_ = skins_.emplace_back();
		const tinygltf::Accessor& transmissionAccessor = transmissionModel.accessors.at(transmissionSkin.inverseBindMatrices);
		const tinygltf::BufferView& transmissionBufferView = transmissionModel.bufferViews.at(transmissionAccessor.bufferView);
		_ASSERT_EXPR(transmissionAccessor.type == TINYGLTF_TYPE_MAT4, L"");

		skin_.inverseBindMatrices_.resize(transmissionAccessor.count);
		std::memcpy(skin_.inverseBindMatrices_.data(), transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset, transmissionAccessor.count * sizeof(DirectX::XMFLOAT4X4));

		skin_.joints_ = transmissionSkin.joints;
	}

	for (std::vector<tinygltf::Animation>::const_reference transmission_animation : transmissionModel.animations)
	{
		Animation& animation{ animations_.emplace_back() };
		animation.name_ = transmission_animation.name;

		for (std::vector<tinygltf::AnimationSampler>::const_reference transmissionSampler : transmission_animation.samplers)
		{
			Animation::Sampler& sampler = animation.samplers_.emplace_back();
			sampler.input_ = transmissionSampler.input;
			sampler.output_ = transmissionSampler.output;
			sampler.interpolation_ = transmissionSampler.interpolation;

			const tinygltf::Accessor& transmissionAccessor = transmissionModel.accessors.at(transmissionSampler.input);
			const tinygltf::BufferView& transmissionBufferView = transmissionModel.bufferViews.at(transmissionAccessor.bufferView);
			_ASSERT_EXPR(transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT, L"");
			_ASSERT_EXPR(transmissionAccessor.type == TINYGLTF_TYPE_SCALAR, L"");
			std::pair<std::unordered_map<int, std::vector<float>>::iterator, bool> keyframeTimes{ animation.keyframeTimes_.emplace(transmissionSampler.input, transmissionAccessor.count) };
			if (keyframeTimes.second)
			{
				std::memcpy(keyframeTimes.first->second.data(), transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset, transmissionAccessor.count * sizeof(FLOAT));
				animation.keyframeTimes_.at(keyframeTimes.first->first).swap(keyframeTimes.first->second);
			}
		}

		for (std::vector<tinygltf::AnimationChannel>::const_reference transmissionChannel : transmission_animation.channels)
		{
			Animation::Channel& channel = animation.channels_.emplace_back();
			channel.sampler_ = transmissionChannel.sampler;
			channel.targetNode_ = transmissionChannel.target_node;
			channel.targetPath_ = transmissionChannel.target_path;

			std::vector<tinygltf::AnimationSampler>::const_reference transmissionSampler = transmission_animation.samplers.at(transmissionChannel.sampler);

			const tinygltf::Accessor& transmissionAccessor = transmissionModel.accessors.at(transmissionSampler.output);
			const tinygltf::BufferView& transmissionBufferView = transmissionModel.bufferViews.at(transmissionAccessor.bufferView);
			if (transmissionChannel.target_path == "scale")
			{
				_ASSERT_EXPR(transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT, L"");
				_ASSERT_EXPR(transmissionAccessor.type == TINYGLTF_TYPE_VEC3, L"");
				std::pair<std::unordered_map<int, std::vector<DirectX::XMFLOAT3>>::iterator, bool> keyframeScales{ animation.keyframeScales_.emplace(transmissionSampler.output, transmissionAccessor.count) };
				if (keyframeScales.second)
				{
					std::memcpy(keyframeScales.first->second.data(), transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset, transmissionAccessor.count * sizeof(DirectX::XMFLOAT3));
					animation.keyframeScales_.at(keyframeScales.first->first).swap(keyframeScales.first->second);
				}
			}
			else if (transmissionChannel.target_path == "rotation")
			{
				_ASSERT_EXPR(transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT, L"");
				_ASSERT_EXPR(transmissionAccessor.type == TINYGLTF_TYPE_VEC4, L"");
				std::pair<std::unordered_map<int, std::vector<DirectX::XMFLOAT4>>::iterator, bool> keyframeRotations{ animation.keyframeRotations_.emplace(transmissionSampler.output, transmissionAccessor.count) };
				if (keyframeRotations.second)
				{
					std::memcpy(keyframeRotations.first->second.data(), transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset, transmissionAccessor.count * sizeof(DirectX::XMFLOAT4));
					animation.keyframeRotations_.at(keyframeRotations.first->first).swap(keyframeRotations.first->second);
				}
			}
			else if (transmissionChannel.target_path == "translation")
			{
				_ASSERT_EXPR(transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT, L"");
				_ASSERT_EXPR(transmissionAccessor.type == TINYGLTF_TYPE_VEC3, L"");
				std::pair<std::unordered_map<int, std::vector<DirectX::XMFLOAT3>>::iterator, bool> keyframeTranslations{ animation.keyframeTranslations_.emplace(transmissionSampler.output, transmissionAccessor.count) };
				if (keyframeTranslations.second)
				{
					std::memcpy(keyframeTranslations.first->second.data(), transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset, transmissionAccessor.count * sizeof(DirectX::XMFLOAT3));
					animation.keyframeTranslations_.at(keyframeTranslations.first->first).swap(keyframeTranslations.first->second);
				}
			}
			else if (transmissionChannel.target_path == "weights")
			{
				//_ASSERT_EXPR(FALSE, L"");
			}
			else
			{
				_ASSERT_EXPR(FALSE, L"");
			}
		}
	}
}

void GeometricSubstance::Animate(size_t animationIndex, float time, std::vector<Node>& animatedNodes, bool loopback, size_t sceneIndex)
{
	_ASSERT_EXPR(animatedNodes.size() > animationIndex, L"");
	_ASSERT_EXPR(animatedNodes.size() == nodes_.size(), L"");

	std::function<size_t(const std::vector<float>&, float, float&, bool)> indexof = [](const std::vector<float>& keyframeTimes, float time, float& interpolationFactor, bool loopback)->size_t {
		size_t keyframeIndex = 0;
		const size_t keyframeCount = keyframeTimes.size();

		const float beginTime = keyframeTimes.at(0);
		const float endTime = keyframeTimes.at(keyframeCount - 1);

		if (time > endTime)
		{
			if (loopback)
			{
				time = fmodf(time, endTime);
			}
			else
			{
				interpolationFactor = 1.0f;
				return keyframeCount - 2;
			}
		}
		else if (time < beginTime)
		{
			interpolationFactor = 0.0f;
			return 0;
		}

		for (size_t timeIndex = 1; timeIndex < keyframeCount; ++timeIndex)
		{
			if (time < keyframeTimes.at(timeIndex))
			{
				keyframeIndex = std::max<size_t>(0LL, timeIndex - 1);
				break;
			}
		}

		interpolationFactor = (time - keyframeTimes.at(keyframeIndex + 0)) / (keyframeTimes.at(keyframeIndex + 1) - keyframeTimes.at(keyframeIndex + 0));

		return keyframeIndex;
	};

	if (animations_.size() > 0)
	{
		const Animation& animation = animations_.at(animationIndex);
		for (std::vector<Animation::Channel>::const_reference channel : animation.channels_)
		{
			if (channel.targetNode_ < 0)
			{
				continue;
			}

			const Animation::Sampler& sampler = animation.samplers_.at(channel.sampler_);
			const std::vector<float>& keyframeTimes = animation.keyframeTimes_.at(sampler.input_);
			if (keyframeTimes.size() == 0)
			{
				continue;
			}
			float interpolation_factor = {};
			size_t keyframe_index = indexof(keyframeTimes, time, interpolation_factor, loopback);
			if (channel.targetPath_ == "scale")
			{
				const std::vector<DirectX::XMFLOAT3>& keyframe_scales = animation.keyframeScales_.at(sampler.output_);
				DirectX::XMStoreFloat3(&animatedNodes.at(channel.targetNode_).scale_, DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&keyframe_scales.at(keyframe_index + 0)), DirectX::XMLoadFloat3(&keyframe_scales.at(keyframe_index + 1)), interpolation_factor));
			}
			else if (channel.targetPath_ == "rotation")
			{
				const std::vector<DirectX::XMFLOAT4>& keyframeRotations = animation.keyframeRotations_.at(sampler.output_);
				DirectX::XMStoreFloat4(&animatedNodes.at(channel.targetNode_).rotation_, DirectX::XMQuaternionNormalize(DirectX::XMQuaternionSlerp(DirectX::XMLoadFloat4(&keyframeRotations.at(keyframe_index + 0)), DirectX::XMLoadFloat4(&keyframeRotations.at(keyframe_index + 1)), interpolation_factor)));
			}
			else if (channel.targetPath_ == "translation")
			{
				const std::vector<DirectX::XMFLOAT3>& keyframeTranslations = animation.keyframeTranslations_.at(sampler.output_);
				DirectX::XMStoreFloat3(&animatedNodes.at(channel.targetNode_).translation_, DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&keyframeTranslations.at(keyframe_index + 0)), DirectX::XMLoadFloat3(&keyframeTranslations.at(keyframe_index + 1)), interpolation_factor));
			}
			else if (channel.targetPath_ == "weights")
			{

			}
			else
			{

			}
		}
		CumulateTransforms(animatedNodes, sceneIndex);
	}
	else
	{
		animatedNodes = nodes_;
	}
}
void GeometricSubstance::CumulateTransforms(std::vector<Node>& nodes_, size_t scene_index)
{
	std::function<void(int, int)> traverse = [&](int parent_index, int node_index)
	{
		Node& Node = nodes_.at(node_index);
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(Node.scale_.x, Node.scale_.y, Node.scale_.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(Node.rotation_.x, Node.rotation_.y, Node.rotation_.z, Node.rotation_.w));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(Node.translation_.x, Node.translation_.y, Node.translation_.z);
		DirectX::XMStoreFloat4x4(&Node.globalTransform_, S * R * T * (parent_index > -1 ? DirectX::XMLoadFloat4x4(&nodes_.at(parent_index).globalTransform_) : DirectX::XMMatrixIdentity()));
		for (decltype(Node.children_)::value_type child_index : Node.children_)
		{
			traverse(node_index, child_index);
		}
	};
	const Scene& Scene = scenes_.at(scene_index);
	for (decltype(Scene.nodes_)::value_type root_node : Scene.nodes_)
	{
		traverse(-1, root_node);
	}
}

