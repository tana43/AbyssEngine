#pragma once

#include <map>
#include <string>
#include <limits>
#include <unordered_map>

#include <d3d11.h>
#include <wrl.h>

#include "MathHelper.h"

#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#include "tinygltf-release/tiny_gltf.h"

#include "ConstantBuffer.h"

namespace AbyssEngine
{
	enum class DrawPass { 
		Opaque,//不透明 
		Transmission, //emissive 
	};

	struct PipelineState
	{
		ID3D11VertexShader* vertexShader_ = NULL;
		ID3D11PixelShader* pixelShader_ = NULL;
		ID3D11DomainShader* domainShader_ = NULL;
		ID3D11HullShader* hullShader_ = NULL;
		ID3D11GeometryShader* geometryShader_ = NULL;
		ID3D11InputLayout* inputLayout_ = NULL;

		ID3D11DepthStencilState* depthStencilState_ = NULL;
		ID3D11BlendState* blendState_ = NULL;
		ID3D11RasterizerState* rasterizerState_ = NULL;
	};

	static constexpr DirectX::XMFLOAT4X4 coordinateSystemTransforms[] = {
			{ -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	// 0:RHS Y-UP
			{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },		// 1:LHS Y-UP
			{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },		// 2:RHS Z-UP
			{ -1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	// 3:LHS Z-UP
	};

	class GeometricSubstance
	{
	protected:
		GeometricSubstance(const std::string& filename) : name_(filename), primitiveConstants_(std::make_unique<decltype(primitiveConstants_)::element_type>()) {}
		GeometricSubstance(const GeometricSubstance& rhs) = delete;
		GeometricSubstance& operator=(const GeometricSubstance& rhs) = delete;
		GeometricSubstance(GeometricSubstance&&) noexcept = delete;
		GeometricSubstance& operator=(GeometricSubstance&&) noexcept = delete;
		~GeometricSubstance() = default;

	public:
		std::string name_;

		struct Asset
		{
			std::string version_ = "2.0";  // required
			std::string generator_;
			std::string minversion_;
			std::string copyright_;
		};
		Asset asset_;

		// KHR_lights_punctual
		// https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_lights_punctual
		struct PunctualLight
		{
			std::string name_;
			float color_[3] = { 1.0f, 1.0f, 1.0f };
			float intensity_ = 1.0f;
			std::string type_;
			float range_ = 0.0f;

			float innerConeAngle_ = 0.0f;
			float outerConeAngle_ = 3.14159265358979f / 4.0f;
		};
		std::vector<PunctualLight> punctualLights_;

		// KHR_materials_variants
		// https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_variants
		std::vector<std::string> variants_;

		struct Scene
		{
			std::string name_;
			std::vector<int> nodes_; // Array of 'root' nodes
		};
		std::vector<Scene> scenes_;

		struct Node
		{
			std::string name_;
			int camera_ = -1;  // the index of camera referenced by this node
			int skin_ = -1;  // the index of skin referenced by this node
			int model_ = -1;  // the index of mesh referenced by this node

			std::vector<int> children_;

			// Local transforms
			DirectX::XMFLOAT4 rotation_ = { 0, 0, 0, 1 };
			DirectX::XMFLOAT3 scale_ = { 1, 1, 1 };
			DirectX::XMFLOAT3 translation_ = { 0, 0, 0 };
			//DirectX::XMFLOAT4X4 matrix = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			std::vector<double> weights_;  // The weights of the instantiated Morph Target

			DirectX::XMFLOAT4X4 globalTransform_ = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

			// The coordinate values of the boundary box (AABB) of a single mesh in the global coordinate system.
			DirectX::XMFLOAT3 minValue_ = { +(std::numeric_limits<float>::max)(), +(std::numeric_limits<float>::max)(), +(std::numeric_limits<float>::max)() };
			DirectX::XMFLOAT3 maxValue_ = { -(std::numeric_limits<float>::max)(), -(std::numeric_limits<float>::max)(), -(std::numeric_limits<float>::max)() };

			bool operator==(const std::string& name_)
			{
				return this->name_ == name_;
			}
		};
		std::vector<Node> nodes_;

		struct BufferView
		{
			DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
			int buffer_ = -1;
			size_t strideInBytes_ = 0;
			size_t sizeInBytes_ = 0;
			size_t Count() const
			{
				return strideInBytes_ > 0 ? sizeInBytes_ / strideInBytes_ : 0;
			}
		};
		std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> buffers_;

		struct Primitive
		{
			int material_ = -1;
			D3D_PRIMITIVE_TOPOLOGY topology_ = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;

			// The coordinate values of the bounding box(AABB) in the local coordinate system.
			DirectX::XMFLOAT3 minValue_ = { +(std::numeric_limits<float>::max)(), +(std::numeric_limits<float>::max)(), +(std::numeric_limits<float>::max)()};
			DirectX::XMFLOAT3 maxValue_ = { -(std::numeric_limits<float>::max)(), -(std::numeric_limits<float>::max)(), -(std::numeric_limits<float>::max)()};

			std::map<std::string, BufferView> vertexBufferViews_;
			BufferView indexBufferView_;

			size_t indexLocation_ = 0;
			size_t indexCount_ = 0;

			bool operator==(const Primitive& rhs)
			{
				return rhs.material_ == material_;
			}
		};
		struct Mesh
		{
			std::string name_;
			std::vector<Primitive> primitives_;
		};
		std::vector<Mesh> meshes_;

		// The coordinate values of the bounding box (AABB) of the entire model in the global coordinate system.
		DirectX::XMFLOAT3 minValue_ = { +FLT_MAX, +FLT_MAX, +FLT_MAX };
		DirectX::XMFLOAT3 maxValue_ = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

		struct Skin
		{
			std::vector<DirectX::XMFLOAT4X4> inverseBindMatrices_;
			std::vector<int> joints_;
		};
		std::vector<Skin> skins_;

		struct Animation
		{
			std::string name_;

			struct Channel
			{
				int sampler_{ -1 }; // required
				int targetNode_{ -1 }; // required (index of the node to target)
				std::string targetPath_; // required in ["translation", "rotation", "scale", "weights"]
			};
			std::vector<Channel> channels_;

			struct Sampler
			{
				int input_{ -1 };
				int output_{ -1 };
				std::string interpolation_;
			};
			std::vector<Sampler> samplers_;

			std::unordered_map<int, std::vector<float>> keyframeTimes_;
			std::unordered_map<int, std::vector<DirectX::XMFLOAT3>> keyframeScales_;
			std::unordered_map<int, std::vector<DirectX::XMFLOAT4>> keyframeRotations_;
			std::unordered_map<int, std::vector<DirectX::XMFLOAT3>> keyframeTranslations_;
		};
		std::vector<Animation> animations_;

		// KHR_texture_transform
		// https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_texture_transform
		struct KhrTextureTransform
		{
			float offset_[2] = { 0, 0 };
			float rotation_ = 0;
			float scale_[2] = { 1, 1 };
			int texcoord_ = 0;

			void init(const tinygltf::ExtensionMap& extensions)
			{
				tinygltf::ExtensionMap::const_iterator extension = extensions.find("KHR_texture_transform");
				if (extension != extensions.end())
				{
					if (extension->second.Has("offset"))
					{
						std::vector<tinygltf::Value>::const_reference value = extension->second.Get("offset");
						offset_[0] = static_cast<float>(value.Get<tinygltf::Value::Array>().at(0).Get<double>());
						offset_[1] = static_cast<float>(value.Get<tinygltf::Value::Array>().at(1).Get<double>());
					}
					if (extension->second.Has("rotation"))
					{
						std::vector<tinygltf::Value>::const_reference value = extension->second.Get("rotation");
						rotation_ = static_cast<float>(value.GetNumberAsDouble());
					}
					if (extension->second.Has("scale"))
					{
						std::vector<tinygltf::Value>::const_reference value = extension->second.Get("scale");
						scale_[0] = static_cast<float>(value.Get<tinygltf::Value::Array>().at(0).Get<double>());
						scale_[1] = static_cast<float>(value.Get<tinygltf::Value::Array>().at(1).Get<double>());
					}
					if (extension->second.Has("texCoord"))
					{
						std::vector<tinygltf::Value>::const_reference value = extension->second.Get("texCoord");
						texcoord_ = value.GetNumberAsInt(); // Overrides the textureInfo texCoord value if supplied, and if this extension is supported.
					}

				}
			}
		};

		// https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo
		struct TextureInfo
		{
			int index_ = -1; // required.
			int texcoord_ = 0; // The set index of texture's TEXCOORD attribute used for texture coordinate mapping.

			KhrTextureTransform khrTextureTransform_;

			void init(const tinygltf::Value::Object& object)
			{
				for (tinygltf::Value::Object::const_reference value : object)
				{
					if (value.first == "index")
					{
						index_ = value.second.GetNumberAsInt();
					}
					else if (value.first == "texCoord")
					{
						texcoord_ = value.second.GetNumberAsInt();
					}
					else if (value.first == "extensions")
					{
						khrTextureTransform_.init(value.second.Get<tinygltf::Value::Object>());
					}
				}
			}
		};
		// https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-material-normaltextureinfo
		struct NormalTextureInfo
		{
			int index_ = -1;  // required
			int texcoord_ = 0;    // The set index of texture's TEXCOORD attribute used for texture coordinate mapping.
			float scale_ = 1;    // scaledNormal = normalize((<sampled normal texture value> * 2.0 - 1.0) * vec3(<normal scale>, <normal scale>, 1.0))

			KhrTextureTransform khrTextureTransform_;

			void init(const tinygltf::Value::Object& object)
			{
				for (tinygltf::Value::Object::const_reference value : object)
				{
					if (value.first == "index")
					{
						index_ = value.second.GetNumberAsInt();
					}
					else if (value.first == "texCoord")
					{
						texcoord_ = value.second.GetNumberAsInt();
					}
					else if (value.first == "scale")
					{
						scale_ = static_cast<float>(value.second.GetNumberAsDouble());
					}
					else if (value.first == "extensions")
					{
						khrTextureTransform_.init(value.second.Get<tinygltf::Value::Object>());
					}
				}
			}
		};
		// https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-material-occlusiontextureinfo
		struct OcclusionTextureInfo
		{
			int index_ = -1;   // required
			int texcoord_ = 0;     // The set index of texture's TEXCOORD attribute used for texture coordinate mapping.
			float strength = 1;  // A scalar parameter controlling the amount of occlusion applied. A value of `0.0` means no occlusion. A value of `1.0` means full occlusion. This value affects the final occlusion value as: `1.0 + strength * (<sampled occlusion texture value> - 1.0)`.

			KhrTextureTransform khr_texture_transform;
		};
		// https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-material-pbrmetallicroughness
		struct PbrMetallicRoughness
		{
			float basecolorFactor_[4] = { 1, 1, 1, 1 };  // len = 4. default [1,1,1,1]
			TextureInfo basecolorTexture_;
			float metallicFactor_ = 1;   // default 1
			float roughnessFactor_ = 1;  // default 1
			TextureInfo metallicRoughnessTexture_;
		};


		// KHR_materials_pbrSpecularGlossiness
		// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Archived/KHR_materials_pbrSpecularGlossiness/README.md
		struct KhrMaterialsPbrSpecularGlossiness
		{
			float diffuseFactor_[4] = { 1, 1, 1, 1 };
			TextureInfo diffuseTexture_;
			float specularFactor_[3] = { 1, 1, 1 };
			float glossinessFactor_ = 1;
			TextureInfo specularGlossinessTexture_;
		};
		// KHR_materials_sheen
		// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_sheen/README.md
		struct KhrMaterialsSheen
		{
			float sheenColorFactor_[3] = { 0, 0, 0 };
			TextureInfo sheenColorTexture_;
			float sheenRoughnessFactor_ = 0;
			TextureInfo sheenRoughnessTexture_;
		};
		// KHR_materials_specular
		// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_specular/README.md
		struct KhrMaterialsSpecular
		{
			float specularFactor_ = 1;
			TextureInfo specularTexture_;
			float specularColorFactor_[3] = { 1, 1, 1 };
			TextureInfo specularColorTexture_;
		};
		// KHR_materials_clearcoat
		// https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_clearcoat
		struct KhrMaterialsClearcoat
		{
			float clearcoatFactor_ = 0;
			TextureInfo clearcoatTexture_;
			float clearcoatRoughnessFactor_ = 0;
			TextureInfo clearcoatRoughnessTexture_;
			NormalTextureInfo clearcoatNormalTexture_;
		};
		// KHR_materials_transmission
		// https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_transmission
		struct KhrMaterialsTransmission
		{
			float transmissionFactor_ = 0;
			TextureInfo transmissionTexture_;
		};
		// KHR_materials_volume
		// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_volume/README.md
		struct KhrMaterialsVolume
		{
			float thicknessFactor_ = 0;
			TextureInfo thicknessTexture_;
			float attenuationDistance_ = 0;
			float attenuationColor_[3] = { 1, 1, 1 };
		};
		// KHR_materials_emissive_strength
		// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_emissive_strength/README.md
		struct KhrMaterialsEmissiveStrength
		{
			float emissiveStrength_ = 1;
		};
		// KHR_materials_ior
		// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_ior/README.md
		struct KhrMaterialsIor
		{
			float ior_ = 1.4f;
		};

		// https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-material
		struct Material {
			std::string name_;
			struct CBuffer
			{
				float emissiveFactor_[3] = { 0, 0, 0 };  // length 3. default [0, 0, 0]
				int alphaMode_ = 0;	// "OPAQUE" : 0, "MASK" : 1, "BLEND" : 2
				float alphaCutoff_ = 0.5f; // default 0.5
				bool doubleSided_ = false; // default false;

				PbrMetallicRoughness pbrMetallicRoughness_;

				NormalTextureInfo normalTexture_;
				OcclusionTextureInfo occlusionTexture_;
				TextureInfo emissiveTexture_;

				// Extensions for glTF 2.0
				KhrMaterialsPbrSpecularGlossiness KhrMaterialsPbrSpecularGlossiness_;
				KhrMaterialsSheen khrMaterialsSheen_;
				KhrMaterialsSpecular khrMaterialsSpecular_;
				KhrMaterialsClearcoat khrMaterialsClearcoat_;
				KhrMaterialsTransmission khrMaterialsTransmission_;
				KhrMaterialsVolume khrMaterialsVolume_;
				KhrMaterialsEmissiveStrength khrMaterialsEmissiveStrength_;
				KhrMaterialsIor khrMaterialsIor_;
			};
			CBuffer data_;

			bool HasOpaque() const
			{
				return data_.khrMaterialsTransmission_.transmissionFactor_ == 0 && data_.alphaMode_ == 0/*OPAQUE*/;
			}
			bool HasTransmission() const
			{
				return data_.khrMaterialsTransmission_.transmissionFactor_ > 0 || data_.alphaMode_ != 0/*OPAQUE*/;
			}
#if 0
			bool has_emissive() const
			{
				return data.emissive_factor[0] != 0 || data.emissive_factor[1] != 0 || data.emissive_factor[2] != 0;
			}
#endif
		};
		std::vector<Material> materials_;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> materialResourceView_;
		// The slot number of 'material_constants' are defined in 'material.hlsli'.
		const int Material_Slot = 0; // t0


		std::vector<std::string> extensionsUsed_;
		std::vector<std::string> extensionsRequired_;

		struct GSTexture
		{
			std::string name_;

			int sampler_ = -1;
			int source_ = -1;
		};
		std::vector<GSTexture> textures_;
		struct Sampler
		{
			int magFilter_ = 0;
			int minFilter_ = 0;
			int wrapS_ = 10497;
			int wrapT_ = 10497;
			std::string name_;
		};
		std::vector<Sampler> samplers_;
		struct Image
		{
			std::string name_;
			int width_ = -1;
			int height_ = -1;
			int component_ = -1;
			int bits_ = -1;			// bit depth per channel. 8(byte), 16 or 32.
			int pixelType_ = -1;	// pixel type(TINYGLTF_COMPONENT_TYPE_***). usually UBYTE(bits = 8) or USHORT(bits = 16)
			int bufferView_;		// (required if no uri)
			std::string mimeType_;	// (required if no uri) ["image/jpeg", "image/png", "image/bmp", "image/gif"]
			std::string uri_;		// (required if no mimeType) uri is not decoded(e.g. whitespace may be represented as %20)

			// When this flag is true, data is stored to `image` in as-is format(e.g. jpeg
			// compressed for "image/jpeg" mime) This feature is good if you use custom
			// image loader function. (e.g. delayed decoding of images for faster glTF
			// parsing) Default parser for Image does not provide as-is loading feature at
			// the moment. (You can manipulate this by providing your own LoadImageData
			// function)
			bool asIs_ = false;
		};
		std::vector<Image> images_;
		std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureResourceViews_;

		//アニメーション関数　アニメーションが終わっているならTrueを返す
		bool Animate(size_t animationIndex, float time, std::vector<Node>& animatedNodes, bool loopback = true, size_t sceneIndex = 0);
		void AppendAnimation(const std::string& filename);
		void AppendAnimations(const std::vector<std::string>& filenames);

		struct PrimitiveConstants
		{
			DirectX::XMFLOAT4X4 transform_ = {};
			DirectX::XMFLOAT4 color_ = {};
			int material_ = -1;
			int startInstanceLocation_ = 0; // Index of start of cascade plane
			int skin_ = -1;
			float emissiveIntensity_ = 15.0f;
			float imageBasedLightingIntensity_ = 1.0f;
		};
		std::unique_ptr<ConstantBuffer<PrimitiveConstants>> primitiveConstants_;
		const int Primitive_Slot = 0; // b0

	protected:
		void ExtractAssets(const tinygltf::Model& transmissionModel);
		void ExtractExtensions(const tinygltf::Model& transmissionModel);
		void ExtractScenes(const tinygltf::Model& transmissionModel);
		void ExtractNodes(const tinygltf::Model& transmissionModel);
		void ExtractMaterials(const tinygltf::Model& transmissionModel);
		void ExtractTextures(const tinygltf::Model& transmissionModel);
		void ExtractAnimations(const tinygltf::Model& transmissionModel);

		void CumulateTransforms(std::vector<Node>& nodes, size_t sceneIndex);
	};

	template <typename T>
	static void ComputeNormals(const DirectX::XMFLOAT3* positions, size_t vertexCount, const T* indices, size_t indexCount, DirectX::XMFLOAT3* normals)
	{
		for (size_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
		{
			normals[vertexIndex].x = 0;
			normals[vertexIndex].y = 0;
			normals[vertexIndex].z = 0;
		}

		for (size_t indexIndex = 0; indexIndex < indexCount; indexIndex += 3)
		{
			const T i1 = indices[indexIndex + 0];
			const T i2 = indices[indexIndex + 1];
			const T i3 = indices[indexIndex + 2];

			const DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&positions[i1]);
			const DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&positions[i2]);
			const DirectX::XMVECTOR v3 = DirectX::XMLoadFloat3(&positions[i3]);

			DirectX::XMFLOAT3 faceNormal;
			DirectX::XMStoreFloat3(&faceNormal, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v2, v1), DirectX::XMVectorSubtract(v3, v1))));

			DirectX::XMStoreFloat3(&normals[i1], DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&faceNormal), DirectX::XMLoadFloat3(&normals[i1])));
			DirectX::XMStoreFloat3(&normals[i2], DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&faceNormal), DirectX::XMLoadFloat3(&normals[i2])));
			DirectX::XMStoreFloat3(&normals[i3], DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&faceNormal), DirectX::XMLoadFloat3(&normals[i3])));
		}

		for (size_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
		{
			DirectX::XMStoreFloat3(&normals[vertexIndex], DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&normals[vertexIndex])));
		}
	}
	template <typename T>
	static void ComputeTangents(const DirectX::XMFLOAT3* positions, const DirectX::XMFLOAT3* normals, const DirectX::XMFLOAT2* texcoords, size_t vertexCount, const T* indices, size_t indexCount, DirectX::XMFLOAT4* tangents)
	{
		std::unique_ptr<DirectX::XMFLOAT3[]> tan1 = std::make_unique<DirectX::XMFLOAT3[]>(vertexCount);
		std::unique_ptr<DirectX::XMFLOAT3[]> tan2 = std::make_unique<DirectX::XMFLOAT3[]>(vertexCount);

		for (size_t indexIndex = 0; indexIndex < indexCount; indexIndex += 3)
		{
			const T i1 = indices[indexIndex + 0];
			const T i2 = indices[indexIndex + 1];
			const T i3 = indices[indexIndex + 2];

			const DirectX::XMFLOAT3 v1 = positions[i1];
			const DirectX::XMFLOAT3 v2 = positions[i2];
			const DirectX::XMFLOAT3 v3 = positions[i3];

			const DirectX::XMFLOAT2 w1 = texcoords[i1];
			const DirectX::XMFLOAT2 w2 = texcoords[i2];
			const DirectX::XMFLOAT2 w3 = texcoords[i3];

			const float x1 = v2.x - v1.x;
			const float x2 = v3.x - v1.x;
			const float y1 = v2.y - v1.y;
			const float y2 = v3.y - v1.y;
			const float z1 = v2.z - v1.z;
			const float z2 = v3.z - v1.z;

			const float s1 = w2.x - w1.x;
			const float s2 = w3.x - w1.x;
			const float t1 = w2.y - w1.y;
			const float t2 = w3.y - w1.y;

			float denominator = s1 * t2 - s2 * t1;
			if (denominator == 0.0f)
			{
				denominator = 1.0f;
			}
			const float r = 1.0f / denominator;

			const DirectX::XMFLOAT3 sdir = { (t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r };
			const DirectX::XMFLOAT3 tdir = { (s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r };

			tan1[i1] = { tan1[i1].x + sdir.x, tan1[i1].y + sdir.y, tan1[i1].z + sdir.z };
			tan1[i2] = { tan1[i2].x + sdir.x, tan1[i2].y + sdir.y, tan1[i2].z + sdir.z };
			tan1[i3] = { tan1[i3].x + sdir.x, tan1[i3].y + sdir.y, tan1[i3].z + sdir.z };

			tan2[i1] = { tan2[i1].x + tdir.x, tan2[i1].y + tdir.y, tan2[i1].z + tdir.z };
			tan2[i2] = { tan2[i2].x + tdir.x, tan2[i2].y + tdir.y, tan2[i2].z + tdir.z };
			tan2[i3] = { tan2[i3].x + tdir.x, tan2[i3].y + tdir.y, tan2[i3].z + tdir.z };
		}

		for (size_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
		{
			DirectX::XMVECTOR n = DirectX::XMLoadFloat3(&normals[vertexIndex]);
			DirectX::XMVECTOR t = DirectX::XMLoadFloat3(&tan1[vertexIndex]);

			// Gram-Schmidt orthogonalize        
			DirectX::XMStoreFloat4(&tangents[vertexIndex], DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(t, DirectX::XMVectorMultiply(n, DirectX::XMVector3Dot(n, t)))));
			// Calculate handedness        
			tangents[vertexIndex].w = (DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Cross(n, t), DirectX::XMLoadFloat3(&tan2[vertexIndex]))) < 0.0f) ? -1.0f : +1.0f;
		}
	}

	inline bool NullLoadImageData(tinygltf::Image*, const int, std::string*, std::string*, int, int, const unsigned char*, int, void*)
	{
		return true;
	}
}