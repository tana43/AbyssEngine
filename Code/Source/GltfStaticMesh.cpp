#include "GltfStaticMesh.h"

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
#include <algorithm>

#include <string.h>

#include "Misc.h"
#include "Shader.h"
#include "Texture.h"
#include "BoundingBox.h"
#include "DXSystem.h"
#include "RenderManager.h"

using namespace DirectX;
using namespace AbyssEngine;

GltfStaticMesh::GltfStaticMesh(const std::string& filename) : GeometricSubstance(filename)
{
	HRESULT hr{ S_OK };

	tinygltf::TinyGLTF tinyGltf;
#if 1
	tinyGltf.SetImageLoader(NullLoadImageData, nullptr);
#endif

	tinygltf::Model transmissionModel;
	std::string error, warning;
	bool succeeded{ false };
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

	ExtractAssets(transmissionModel);
	ExtractExtensions(transmissionModel);

	ExtractScenes(transmissionModel);
	ExtractNodes(transmissionModel);

	ExtractMaterials(transmissionModel);
	ExtractTextures(transmissionModel);
	ExtractMeshes(transmissionModel);

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	staticMeshVs_ = Shader<ID3D11VertexShader>::Emplace("./Resources/Shader/StaticMeshVS.cso",inputLayout_.ReleaseAndGetAddressOf(), inputElementDesc, _countof(inputElementDesc));
#if ENABLE_DIFFERD_RENDERING
	staticMeshPs_ =  Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/GeometricSubstanceGBufferPS.cso");
#else
	staticMeshPs_ =  Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/GeometricSubstancePS.cso");
#endif

	D3D11_INPUT_ELEMENT_DESC csmOpaqueInputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	csmOpaqueStaticMeshVs_ = Shader<ID3D11VertexShader>::Emplace("./Resources/Shader/CsmOpaqueStaticMeshVS.cso", csmOpaquInputLayout_.ReleaseAndGetAddressOf(), csmOpaqueInputElementDesc, _countof(csmOpaqueInputElementDesc));
	csmOpaqueGs_ = Shader<ID3D11GeometryShader>::Emplace("./Resources/Shader/CsmOpaqueGS.cso");

	D3D11_INPUT_ELEMENT_DESC csmTransparentInputElementDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	csmTransparentStaticMeshVs_ = Shader<ID3D11VertexShader>::Emplace("./Resources/Shader/CsmTransparentStaticMeshVS.cso",csmTransparentInputLayout_.ReleaseAndGetAddressOf(), csmTransparentInputElementDesc, _countof(csmTransparentInputElementDesc));
	csmTransparentGs_ = Shader<ID3D11GeometryShader>::Emplace("./Resources/Shader/CsmTransparentGS.cso");
	csmTransparentPs_ = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/CsmTransparentPS.cso");
}
void GltfStaticMesh::ExtractMeshes(const tinygltf::Model& transmissionModel)
{
	ID3D11Device* device = DXSystem::GetDevice();

	HRESULT hr = S_OK;

	struct CombinedBuffer
	{
		size_t indexCount_;
		size_t vertexCount_;

		D3D_PRIMITIVE_TOPOLOGY topology_ = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		std::vector<unsigned int> indices_;
		struct StructureOfArrays
		{
			std::vector<XMFLOAT3> positions_;
			std::vector<XMFLOAT3> normals_;
			std::vector<XMFLOAT4> tangents_;
			std::vector<XMFLOAT2> texcoords0_;
			std::vector<XMFLOAT2> texcoords1_;
		};
		StructureOfArrays vertices_;
		XMFLOAT3 minValue_ = { +FLT_MAX, +FLT_MAX, +FLT_MAX };
		XMFLOAT3 maxValue_ = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	};
	std::unordered_map<int/*material*/, CombinedBuffer> combinedBuffers_;

#if 1
	// Pre-calculation of buffer sizes
	for (decltype(transmissionModel.nodes)::const_reference transmissionNode : transmissionModel.nodes)
	{
		if (transmissionNode.mesh > -1)
		{
			const tinygltf::Mesh& transmissionMesh = transmissionModel.meshes.at(transmissionNode.mesh);
			for (decltype(transmissionMesh.primitives)::const_reference transmissionPrimitive : transmissionMesh.primitives)
			{
				if (transmissionPrimitive.indices > -1)
				{
					combinedBuffers_[transmissionPrimitive.material].indexCount_ += transmissionModel.accessors.at(transmissionPrimitive.indices).count;
				}
				if (transmissionPrimitive.attributes.at("POSITION") > -1)
				{
					combinedBuffers_[transmissionPrimitive.material].vertexCount_ += transmissionModel.accessors.at(transmissionPrimitive.attributes.at("POSITION")).count;
				}
			}
		}
	}
	for (decltype(combinedBuffers_)::reference combinedBuffer_ : combinedBuffers_)
	{
		combinedBuffer_.second.indices_.reserve(combinedBuffer_.second.indexCount_);
		combinedBuffer_.second.vertices_.positions_.reserve(combinedBuffer_.second.vertexCount_);
		combinedBuffer_.second.vertices_.normals_.reserve(combinedBuffer_.second.vertexCount_);
		combinedBuffer_.second.vertices_.tangents_.reserve(combinedBuffer_.second.vertexCount_);
		combinedBuffer_.second.vertices_.texcoords0_.reserve(combinedBuffer_.second.vertexCount_);
		combinedBuffer_.second.vertices_.texcoords1_.reserve(combinedBuffer_.second.vertexCount_);
	}
#endif

	for (decltype(nodes_)::reference node : nodes_)
	{
		//const DirectX::XMFLOAT4X4 coordinateSystemTransforms[] = {
		//	{ -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	// 0:RHS Y-UP
		//	{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },		// 1:LHS Y-UP
		//	{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },		// 2:RHS Z-UP
		//	{ -1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	// 3:LHS Z-UP
		//};
		const XMMATRIX globalTransform = XMLoadFloat4x4(&node.globalTransform_) * XMLoadFloat4x4(&coordinateSystemTransforms[1]);

		if (node.model_ > -1 /*&& node.skin < 0*/)
		{
			const tinygltf::Mesh& transmissionMesh = transmissionModel.meshes.at(node.model_);

			// Assign a new mesh index to the node.
			node.model_ = static_cast<int>(meshes_.size());

			Mesh& mesh = meshes_.emplace_back();
			mesh.name_ = transmissionMesh.name;

			for (std::vector<tinygltf::Primitive>::const_reference transmissionPrimitive : transmissionMesh.primitives)
			{
				Primitive& primitive = mesh.primitives_.emplace_back();
				primitive.material_ = transmissionPrimitive.material;
#if 0
				_ASSERT_EXPR(transmissionPrimitive.mode == TINYGLTF_MODE_TRIANGLES, L"Only TINYGLTF_MODE_TRIANGLES is supported for primitive topologies.");
#else
				const std::unordered_map<size_t, D3D_PRIMITIVE_TOPOLOGY> topologies =
				{
					{ TINYGLTF_MODE_TRIANGLES, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST },
					{ TINYGLTF_MODE_TRIANGLE_STRIP, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP },
					{ TINYGLTF_MODE_POINTS, D3D_PRIMITIVE_TOPOLOGY_POINTLIST },
					{ TINYGLTF_MODE_LINE, D3D_PRIMITIVE_TOPOLOGY_LINELIST },
					{ TINYGLTF_MODE_LINE_STRIP, D3D_PRIMITIVE_TOPOLOGY_LINESTRIP },
				};
				primitive.topology_ = topologies.at(transmissionPrimitive.mode);
#endif
				CombinedBuffer& combinedBuffer_ = combinedBuffers_[transmissionPrimitive.material];
				if (combinedBuffer_.topology_ == D3D_PRIMITIVE_TOPOLOGY_UNDEFINED)
				{
					combinedBuffer_.topology_ = topologies.at(transmissionPrimitive.mode);
				}
				else
				{
					_ASSERT_EXPR(combinedBuffer_.topology_ == topologies.at(transmissionPrimitive.mode), L"The topology of all merged primitives must be identical.");
				}

				// Combine primitives using the same material into a single index buffer.
				if (transmissionPrimitive.indices > -1)
				{
					const tinygltf::Accessor transmissionAccessor = transmissionModel.accessors.at(transmissionPrimitive.indices);
					const tinygltf::BufferView& transmissionBufferView = transmissionModel.bufferViews.at(transmissionAccessor.bufferView);

					if (transmissionAccessor.count == 0)
					{
						continue;
					}

					primitive.indexLocation_ = combinedBuffer_.indices_.size();
					primitive.indexCount_ = transmissionAccessor.count;

					const size_t vertexOffset_ = combinedBuffer_.vertices_.positions_.size();
					if (transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
					{
						const unsigned char* buffer = transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset;
						for (size_t accessor_index = 0; accessor_index < transmissionAccessor.count; ++accessor_index)
						{
							combinedBuffer_.indices_.emplace_back(static_cast<unsigned int>(buffer[accessor_index] + vertexOffset_));
						}
					}
					else if (transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
					{
						const unsigned short* buffer = reinterpret_cast<const unsigned short*>(transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset);
						for (size_t accessor_index = 0; accessor_index < transmissionAccessor.count; ++accessor_index)
						{
							combinedBuffer_.indices_.emplace_back(static_cast<unsigned int>(buffer[accessor_index] + vertexOffset_));
						}
					}
					else if (transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
					{
						const unsigned int* buffer = reinterpret_cast<const unsigned int*>(transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset);
						for (size_t accessor_index = 0; accessor_index < transmissionAccessor.count; ++accessor_index)
						{
							combinedBuffer_.indices_.emplace_back(static_cast<unsigned int>(buffer[accessor_index] + vertexOffset_));
						}
					}
					else
					{
						_ASSERT_EXPR(false, L"This index format is not supported.");
					}
				}

				// Combine primitives using the same material into a single vertex buffer. In addition, apply a coordinate transformation to position, normal and tangent of primitives.
				for (decltype(transmissionPrimitive.attributes)::const_reference transmission_attribute : transmissionPrimitive.attributes)
				{
					const tinygltf::Accessor transmissionAccessor = transmissionModel.accessors.at(transmission_attribute.second);
					const tinygltf::BufferView& transmissionBufferView = transmissionModel.bufferViews.at(transmissionAccessor.bufferView);

					if (transmissionAccessor.count == 0)
					{
						continue;
					}

					if (transmission_attribute.first == "POSITION")
					{
						_ASSERT_EXPR(transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && transmissionAccessor.type == TINYGLTF_TYPE_VEC3, L"'POSITION' attribute must be of type TINYGLTF_COMPONENT_TYPE_FLOAT & TINYGLTF_TYPE_VEC3.");
						const XMFLOAT3* buffer = reinterpret_cast<const XMFLOAT3*>(transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset);
						for (size_t accessor_index = 0; accessor_index < transmissionAccessor.count; ++accessor_index)
						{
							XMFLOAT3 position_ = buffer[accessor_index];
							DirectX::XMStoreFloat3(&position_, XMVector3TransformCoord(XMLoadFloat3(&position_), globalTransform));
							combinedBuffer_.vertices_.positions_.emplace_back(position_);
						}
#if 1
						// Constructing a bounding box
						if (transmissionAccessor.maxValues.size() == 3 && transmissionAccessor.minValues.size() == 3)
						{
							XMVECTOR min_value = XMVectorSet(static_cast<float>(transmissionAccessor.minValues.at(0)), static_cast<float>(transmissionAccessor.minValues.at(1)), static_cast<float>(transmissionAccessor.minValues.at(2)), 1.0f);
							XMVECTOR max_value = XMVectorSet(static_cast<float>(transmissionAccessor.maxValues.at(0)), static_cast<float>(transmissionAccessor.maxValues.at(1)), static_cast<float>(transmissionAccessor.maxValues.at(2)), 1.0f);

							DirectX::XMStoreFloat3(&primitive.maxValue_, max_value);
							DirectX::XMStoreFloat3(&primitive.minValue_, min_value);

							ComputeTransformedBounds(min_value, max_value, globalTransform);
							DirectX::XMStoreFloat3(&combinedBuffer_.maxValue_, XMVectorMax(XMLoadFloat3(&combinedBuffer_.maxValue_), max_value));
							DirectX::XMStoreFloat3(&combinedBuffer_.minValue_, XMVectorMin(XMLoadFloat3(&combinedBuffer_.minValue_), min_value));
						}
#endif
					}
					else if (transmission_attribute.first == "NORMAL")
					{
						_ASSERT_EXPR(transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && transmissionAccessor.type == TINYGLTF_TYPE_VEC3, L"'NORMAL' attribute must be of type TINYGLTF_COMPONENT_TYPE_FLOAT & TINYGLTF_TYPE_VEC3.");
						const XMFLOAT3* buffer = reinterpret_cast<const XMFLOAT3*>(transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset);
						for (size_t accessor_index = 0; accessor_index < transmissionAccessor.count; ++accessor_index)
						{
							XMFLOAT3 normal = buffer[accessor_index];
							XMStoreFloat3(&normal, XMVector3TransformNormal(XMLoadFloat3(&normal), globalTransform));
							combinedBuffer_.vertices_.normals_.emplace_back(normal);
						}
					}
					else if (transmission_attribute.first == "TANGENT")
					{
						_ASSERT_EXPR(transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && transmissionAccessor.type == TINYGLTF_TYPE_VEC4, L"'TANGENT' attribute must be of type TINYGLTF_COMPONENT_TYPE_FLOAT & TINYGLTF_TYPE_VEC4.");
						const XMFLOAT4* buffer = reinterpret_cast<const XMFLOAT4*>(transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset);
						for (size_t accessor_index = 0; accessor_index < transmissionAccessor.count; ++accessor_index)
						{
							XMFLOAT4 tangent = buffer[accessor_index];
							float sigma = tangent.w;
							tangent.w = 0;
							XMStoreFloat4(&tangent, XMVector4Transform(XMLoadFloat4(&tangent), globalTransform));
							tangent.w = sigma;
							combinedBuffer_.vertices_.tangents_.emplace_back(tangent);
						}
					}
					else if (transmission_attribute.first == "TEXCOORD_0")
					{
						_ASSERT_EXPR(transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && transmissionAccessor.type == TINYGLTF_TYPE_VEC2, L"'TEXCOORD_0' attribute must be of type TINYGLTF_COMPONENT_TYPE_FLOAT & TINYGLTF_TYPE_VEC2.");
						const XMFLOAT2* buffer = reinterpret_cast<const XMFLOAT2*>(transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset);
						for (size_t accessor_index = 0; accessor_index < transmissionAccessor.count; ++accessor_index)
						{
							combinedBuffer_.vertices_.texcoords0_.emplace_back(buffer[accessor_index]);
						}
					}
					else if (transmission_attribute.first == "TEXCOORD_1")
					{
						_ASSERT_EXPR(transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && transmissionAccessor.type == TINYGLTF_TYPE_VEC2, L"'TEXCOORD_1' attribute must be of type TINYGLTF_COMPONENT_TYPE_FLOAT & TINYGLTF_TYPE_VEC2.");
						const XMFLOAT2* buffer = reinterpret_cast<const XMFLOAT2*>(transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset);
						for (size_t accessor_index = 0; accessor_index < transmissionAccessor.count; ++accessor_index)
						{
							combinedBuffer_.vertices_.texcoords1_.emplace_back(buffer[accessor_index]);
						}
					}
				}


#if 1
				std::function<std::tuple<const void*, size_t>(const std::string& attribute)> addressof = [&](const std::string& attribute)->std::tuple<const void*, size_t>
				{
					_ASSERT_EXPR(transmissionPrimitive.attributes.find(attribute) != transmissionPrimitive.attributes.end(), L"");

					std::tuple<const void*, size_t> tuple_value;

					std::vector<tinygltf::Accessor>::const_reference transmissionAccessor = transmissionModel.accessors.at(transmissionPrimitive.attributes.at(attribute));
					std::vector<tinygltf::BufferView>::const_reference transmissionBufferView = transmissionModel.bufferViews.at(transmissionAccessor.bufferView);

					std::get<0>(tuple_value) = transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset;
					std::get<1>(tuple_value) = transmissionAccessor.count;

					return tuple_value;
				};
				// If this primitive does not have normals, calculate them.
				if (transmissionPrimitive.attributes.find("NORMAL") == transmissionPrimitive.attributes.end())
				{
					std::tuple<const void*, size_t> tuple_value = addressof("POSITION");
					const DirectX::XMFLOAT3* positions = reinterpret_cast<const DirectX::XMFLOAT3*>(std::get<0>(tuple_value));
					const size_t vertex_count = std::get<1>(tuple_value);

					std::vector<DirectX::XMFLOAT3> normals(vertex_count);

					_ASSERT_EXPR(transmissionPrimitive.indices > -1, L"An index buffer is needed for this process.");
					std::vector<tinygltf::Accessor>::const_reference transmissionAccessor = transmissionModel.accessors.at(transmissionPrimitive.indices);
					std::vector<tinygltf::BufferView>::const_reference transmissionBufferView = transmissionModel.bufferViews.at(transmissionAccessor.bufferView);
					const void* indices = transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset;
					size_t index_count = transmissionAccessor.count;

					if (transmissionModel.accessors.at(transmissionPrimitive.indices).componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
					{
						ComputeNormals<unsigned int>(positions, vertex_count, reinterpret_cast<const unsigned int*>(indices), index_count, normals.data());
					}
					else if (transmissionModel.accessors.at(transmissionPrimitive.indices).componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
					{
						ComputeNormals<unsigned short>(positions, vertex_count, reinterpret_cast<const unsigned short*>(indices), index_count, normals.data());
					}
					else
					{
						_ASSERT_EXPR(FALSE, L"This index format is not supported.");
					}

					for (decltype(normals)::value_type normal : normals)
					{
						combinedBuffer_.vertices_.normals_.emplace_back(normal);
					}
				}
				// If this primitive does not have tangents, calculate them.
				if (transmissionPrimitive.attributes.find("TANGENT") == transmissionPrimitive.attributes.end() && transmissionPrimitive.attributes.find("TEXCOORD_0") != transmissionPrimitive.attributes.end())
				{
					std::tuple<const void*, size_t> tuple_value;

					tuple_value = addressof("POSITION");
					const DirectX::XMFLOAT3* positions = reinterpret_cast<const DirectX::XMFLOAT3*>(std::get<0>(tuple_value));
					const size_t vertex_count = std::get<1>(tuple_value);

					const DirectX::XMFLOAT3* normals = {};
					if (transmissionPrimitive.attributes.find("NORMAL") == transmissionPrimitive.attributes.end())
					{
						_ASSERT_EXPR(transmissionPrimitive.indices > -1, L"An index buffer is needed for this process.");
						_ASSERT_EXPR(combinedBuffer_.vertices_.normals_.size() > 0, L"Normal attribute of vertex buffer is needed for this process.");
						normals = combinedBuffer_.vertices_.normals_.data();
					}
					else
					{
						tuple_value = addressof("NORMAL");
						normals = reinterpret_cast<const DirectX::XMFLOAT3*>(std::get<0>(tuple_value));
						_ASSERT_EXPR(vertex_count == std::get<1>(tuple_value), L"");
					}

					tuple_value = addressof("TEXCOORD_0");
					const DirectX::XMFLOAT2* texcoords = reinterpret_cast<const DirectX::XMFLOAT2*>(std::get<0>(tuple_value));
					_ASSERT_EXPR(vertex_count == std::get<1>(tuple_value), L"");

					_ASSERT_EXPR(transmissionPrimitive.indices > -1, L"An index buffer is needed for this process.");
					std::vector<tinygltf::Accessor>::const_reference transmissionAccessor{ transmissionModel.accessors.at(transmissionPrimitive.indices) };
					std::vector<tinygltf::BufferView>::const_reference transmissionBufferView{ transmissionModel.bufferViews.at(transmissionAccessor.bufferView) };
					const void* indices = transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset;
					size_t index_count = transmissionAccessor.count;

					std::vector<DirectX::XMFLOAT4> tangents(vertex_count);

					if (transmissionModel.accessors.at(transmissionPrimitive.indices).componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
					{
						ComputeTangents<unsigned int>(positions, normals, texcoords, vertex_count, reinterpret_cast<const unsigned int*>(indices), index_count, tangents.data());
					}
					else if (transmissionModel.accessors.at(transmissionPrimitive.indices).componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
					{
						ComputeTangents<unsigned short>(positions, normals, texcoords, vertex_count, reinterpret_cast<const unsigned short*>(indices), index_count, tangents.data());
					}
					else
					{
						_ASSERT_EXPR(FALSE, L"This index format is not supported.");
					}

					for (decltype(tangents)::value_type tangent : tangents)
					{
						combinedBuffer_.vertices_.tangents_.emplace_back(tangent);
					}
				}
#endif
			}
		}
	}

	for (decltype(combinedBuffers_)::const_reference combinedBuffer_ : combinedBuffers_)
	{
#if 1
		if (combinedBuffer_.second.vertices_.positions_.size() == 0)
		{
			continue;
		}
#endif
		Primitive& batchPrimitive = batchPrimitives_.emplace_back();
		batchPrimitive.material_ = combinedBuffer_.first;
		batchPrimitive.topology_ = combinedBuffer_.second.topology_;
		batchPrimitive.maxValue_ = combinedBuffer_.second.maxValue_;
		batchPrimitive.minValue_ = combinedBuffer_.second.minValue_;

		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		if (combinedBuffer_.second.indices_.size() > 0)
		{
			batchPrimitive.indexBufferView_.format_ = DXGI_FORMAT_R32_UINT;
			batchPrimitive.indexBufferView_.strideInBytes_ = sizeof(UINT);
			batchPrimitive.indexBufferView_.sizeInBytes_ = combinedBuffer_.second.indices_.size() * batchPrimitive.indexBufferView_.strideInBytes_;
			batchPrimitive.indexBufferView_.buffer_ = static_cast<int>(buffers_.size());
			batchPrimitive.indexLocation_ = 0;
			batchPrimitive.indexCount_ = combinedBuffer_.second.indices_.size();

			bufferDesc.ByteWidth = static_cast<UINT>(batchPrimitive.indexBufferView_.sizeInBytes_);
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			subresource_data.pSysMem = combinedBuffer_.second.indices_.data();
			subresource_data.SysMemPitch = 0;
			subresource_data.SysMemSlicePitch = 0;
			hr = device->CreateBuffer(&bufferDesc, &subresource_data, buffers_.emplace_back().ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		BufferView vertexBufferView;
		if (combinedBuffer_.second.vertices_.positions_.size() > 0)
		{
			vertexBufferView.format_ = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexBufferView.strideInBytes_ = sizeof(FLOAT) * 3;
			vertexBufferView.sizeInBytes_ = combinedBuffer_.second.vertices_.positions_.size() * vertexBufferView.strideInBytes_;
			vertexBufferView.buffer_ = static_cast<int>(buffers_.size());

			bufferDesc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			subresource_data.pSysMem = combinedBuffer_.second.vertices_.positions_.data();
			hr = device->CreateBuffer(&bufferDesc, &subresource_data, buffers_.emplace_back().ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
			batchPrimitive.vertexBufferViews_.emplace(std::make_pair("POSITION", vertexBufferView));
		}
		if (combinedBuffer_.second.vertices_.normals_.size() > 0)
		{
			vertexBufferView.format_ = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexBufferView.strideInBytes_ = sizeof(FLOAT) * 3;
			vertexBufferView.sizeInBytes_ = combinedBuffer_.second.vertices_.normals_.size() * vertexBufferView.strideInBytes_;
			vertexBufferView.buffer_ = static_cast<int>(buffers_.size());

			bufferDesc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			subresource_data.pSysMem = combinedBuffer_.second.vertices_.normals_.data();
			hr = device->CreateBuffer(&bufferDesc, &subresource_data, buffers_.emplace_back().ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
			batchPrimitive.vertexBufferViews_.emplace(std::make_pair("NORMAL", vertexBufferView));
		}
		if (combinedBuffer_.second.vertices_.tangents_.size() > 0)
		{
			vertexBufferView.format_ = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexBufferView.strideInBytes_ = sizeof(FLOAT) * 4;
			vertexBufferView.sizeInBytes_ = combinedBuffer_.second.vertices_.tangents_.size() * vertexBufferView.strideInBytes_;
			vertexBufferView.buffer_ = static_cast<int>(buffers_.size());

			bufferDesc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			subresource_data.pSysMem = combinedBuffer_.second.vertices_.tangents_.data();
			hr = device->CreateBuffer(&bufferDesc, &subresource_data, buffers_.emplace_back().ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
			batchPrimitive.vertexBufferViews_.emplace(std::make_pair("TANGENT", vertexBufferView));
		}
		if (combinedBuffer_.second.vertices_.texcoords0_.size() > 0)
		{
			vertexBufferView.format_ = DXGI_FORMAT_R32G32_FLOAT;
			vertexBufferView.strideInBytes_ = sizeof(FLOAT) * 2;
			vertexBufferView.sizeInBytes_ = combinedBuffer_.second.vertices_.texcoords0_.size() * vertexBufferView.strideInBytes_;
			vertexBufferView.buffer_ = static_cast<int>(buffers_.size());

			bufferDesc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			subresource_data.pSysMem = combinedBuffer_.second.vertices_.texcoords0_.data();
			hr = device->CreateBuffer(&bufferDesc, &subresource_data, buffers_.emplace_back().ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
			batchPrimitive.vertexBufferViews_.emplace(std::make_pair("TEXCOORD_0", vertexBufferView));
		}
		if (combinedBuffer_.second.vertices_.texcoords1_.size() > 0)
		{
			vertexBufferView.format_ = DXGI_FORMAT_R32G32_FLOAT;
			vertexBufferView.strideInBytes_ = sizeof(FLOAT) * 2;
			vertexBufferView.sizeInBytes_ = combinedBuffer_.second.vertices_.texcoords1_.size() * vertexBufferView.strideInBytes_;
			vertexBufferView.buffer_ = static_cast<int>(buffers_.size());

			bufferDesc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			subresource_data.pSysMem = combinedBuffer_.second.vertices_.texcoords1_.data();
			hr = device->CreateBuffer(&bufferDesc, &subresource_data, buffers_.emplace_back().ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
			batchPrimitive.vertexBufferViews_.emplace(std::make_pair("TEXCOORD_1", vertexBufferView));
		}

#if 1
		_ASSERT_EXPR(batchPrimitive.vertexBufferViews_.find("POSITION") != batchPrimitive.vertexBufferViews_.end(), L"POSITION is required for our vertex attributes.");
#endif
		// Add dummy attributes if any are missing.
		const std::unordered_map<std::string, BufferView> attributes =
		{
			{ "POSITION", { DXGI_FORMAT_R32G32B32_FLOAT } },
			{ "NORMAL", { DXGI_FORMAT_R32G32B32_FLOAT } },
			{ "TANGENT", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
			{ "TEXCOORD_0", { DXGI_FORMAT_R32G32_FLOAT } },
			{ "TEXCOORD_1", { DXGI_FORMAT_R32G32_FLOAT } },
		};
		for (std::unordered_map<std::string, BufferView>::const_reference attribute : attributes)
		{
			if (batchPrimitive.vertexBufferViews_.find(attribute.first) == batchPrimitive.vertexBufferViews_.end())
			{
				batchPrimitive.vertexBufferViews_.insert(std::make_pair(attribute.first, attribute.second));
			}
		}
	}
	for (decltype(meshes_)::reference mesh : meshes_)
	{
		for (decltype(mesh.primitives_)::reference primitive : mesh.primitives_)
		{
			decltype(batchPrimitives_)::iterator batchPrimitive = std::find(batchPrimitives_.begin(), batchPrimitives_.end(), primitive);
			if (batchPrimitive != batchPrimitives_.end())
			{
				primitive.indexBufferView_ = batchPrimitive->indexBufferView_;
				primitive.vertexBufferViews_ = batchPrimitive->vertexBufferViews_;
			}
		}
	}
	// Rebuild bounding boxes (min_value / man_value) computationally.
	for (decltype(nodes_)::reference node : nodes_)
	{
		if (node.model_ > -1)
		{
			DirectX::XMMATRIX globalTransform_ = DirectX::XMLoadFloat4x4(&node.globalTransform_);
			const Mesh& mesh = meshes_.at(node.model_);
			for (std::vector<Primitive>::const_reference primitive : mesh.primitives_)
			{
				DirectX::XMVECTOR maxValue = DirectX::XMLoadFloat3(&primitive.maxValue_);
				DirectX::XMVECTOR minValue = DirectX::XMLoadFloat3(&primitive.minValue_);
				ComputeTransformedBounds(minValue, maxValue, globalTransform_);

				DirectX::XMStoreFloat3(&node.maxValue_, DirectX::XMVectorMax(XMLoadFloat3(&node.maxValue_), maxValue));
				DirectX::XMStoreFloat3(&node.minValue_, DirectX::XMVectorMin(XMLoadFloat3(&node.minValue_), minValue));
			}
			DirectX::XMStoreFloat3(&maxValue_, DirectX::XMVectorMax(DirectX::XMLoadFloat3(&maxValue_), DirectX::XMLoadFloat3(&node.maxValue_)));
			DirectX::XMStoreFloat3(&minValue_, DirectX::XMVectorMin(DirectX::XMLoadFloat3(&minValue_), DirectX::XMLoadFloat3(&node.minValue_)));
		}
		else
		{
			node.maxValue_ = node.minValue_ = { node.globalTransform_._41, node.globalTransform_._42, node.globalTransform_._43 };
		}
	}
}

//int GltfStaticMesh::Draw(DrawPass pass, const DirectX::XMFLOAT4X4& transform, std::function<void(const Material&, PipelineState&)> drawcallback)
//{
//	ID3D11DeviceContext* deviceContext = DXSystem::deviceContext_.Get();
//
//	int drawcallCount = 0;
//
//	// The slot number of 'material_constants' are defined in 'material.hlsli'.
//	const int materialConstantsSlot = 0;
//	deviceContext->PSSetShaderResources(materialConstantsSlot, 1, materialResourceView_.GetAddressOf());
//
//	for (std::vector<Primitive>::const_reference primitive : batchPrimitives_)
//	{
//		if (primitive.vertexBufferViews_.at("POSITION").Count() == 0 || primitive.vertexBufferViews_.at("POSITION").buffer_ < 0)
//		{
//			continue;
//		}
//
//		const Material& material = materials_.at(primitive.material_ < 0 ? materials_.size() - 1 : primitive.material_);
//		_ASSERT_EXPR(material.data_.normalTexture_.index_ > -1 ? primitive.vertexBufferViews_.at("TANGENT").buffer_ > -1 : TRUE, L"");
//
//#if 1
//		bool permitted = false;
//		switch (pass)
//		{
//		case DrawPass::Opaque:
//			permitted = material.HasOpaque();
//			break;
//		case DrawPass::Transmission:
//			permitted = material.HasTransmission();
//			break;
//		}
//		if (!permitted)
//		{
//			continue;
//		}
//#endif
//
//		PipelineState replacementPipelineState = {};
//		drawcallback(material, replacementPipelineState);
//		deviceContext->VSSetShader(replacementPipelineState.vertexShader_ ? replacementPipelineState.vertexShader_ : staticMeshVs_.Get(), NULL, 0);
//		deviceContext->PSSetShader(replacementPipelineState.pixelShader_ ? replacementPipelineState.pixelShader_ : staticMeshPs_.Get(), NULL, 0);
//		deviceContext->DSSetShader(replacementPipelineState.domainShader_ ? replacementPipelineState.domainShader_ : NULL, NULL, 0);
//		deviceContext->HSSetShader(replacementPipelineState.hullShader_ ? replacementPipelineState.hullShader_ : NULL, NULL, 0);
//		deviceContext->GSSetShader(replacementPipelineState.geometryShader_ ? replacementPipelineState.geometryShader_ : NULL, NULL, 0);
//		deviceContext->IASetInputLayout(replacementPipelineState.inputLayout_ ? replacementPipelineState.inputLayout_ : inputLayout_.Get());
//		if (replacementPipelineState.depthStencilState_)
//		{
//			deviceContext->OMSetDepthStencilState(replacementPipelineState.depthStencilState_, 0);
//		}
//		if (replacementPipelineState.rasterizerState_)
//		{
//			deviceContext->RSSetState(replacementPipelineState.rasterizerState_);
//		}
//		if (replacementPipelineState.blendState_)
//		{
//			deviceContext->OMSetBlendState(replacementPipelineState.blendState_, NULL, 0xFFFFFFFF);
//		}
//
//		ID3D11Buffer* vertexBuffers[] = {
//			primitive.vertexBufferViews_.at("POSITION").buffer_ > -1 ? buffers_.at(primitive.vertexBufferViews_.at("POSITION").buffer_).Get() : NULL,
//			primitive.vertexBufferViews_.at("NORMAL").buffer_ > -1 ? buffers_.at(primitive.vertexBufferViews_.at("NORMAL").buffer_).Get() : NULL,
//			primitive.vertexBufferViews_.at("TANGENT").buffer_ > -1 ? buffers_.at(primitive.vertexBufferViews_.at("TANGENT").buffer_).Get() : NULL,
//			primitive.vertexBufferViews_.at("TEXCOORD_0").buffer_ > -1 ? buffers_.at(primitive.vertexBufferViews_.at("TEXCOORD_0").buffer_).Get() : NULL,
//			primitive.vertexBufferViews_.at("TEXCOORD_1").buffer_ > -1 ? buffers_.at(primitive.vertexBufferViews_.at("TEXCOORD_1").buffer_).Get() : NULL,
//		};
//		UINT strides[] =
//		{
//			static_cast<UINT>(primitive.vertexBufferViews_.at("POSITION").strideInBytes_),
//			static_cast<UINT>(primitive.vertexBufferViews_.at("NORMAL").strideInBytes_),
//			static_cast<UINT>(primitive.vertexBufferViews_.at("TANGENT").strideInBytes_),
//			static_cast<UINT>(primitive.vertexBufferViews_.at("TEXCOORD_0").strideInBytes_),
//			static_cast<UINT>(primitive.vertexBufferViews_.at("TEXCOORD_1").strideInBytes_),
//		};
//
//		UINT offsets[_countof(vertexBuffers)] = { 0 };
//		deviceContext->IASetVertexBuffers(0, _countof(vertexBuffers), vertexBuffers, strides, offsets);
//
//		// The slot numbers of 'primitive_textures' are defined in 'material.hlsli'.
//		const int textureIndices_[] =
//		{
//			material.data_.pbrMetallicRoughness_.basecolorTexture_.index_,
//			material.data_.pbrMetallicRoughness_.metallicRoughnessTexture_.index_,
//			material.data_.normalTexture_.index_,
//			material.data_.emissiveTexture_.index_,
//			material.data_.occlusionTexture_.index_,
//			material.data_.KhrMaterialsPbrSpecularGlossiness_.diffuseTexture_.index_,
//			material.data_.KhrMaterialsPbrSpecularGlossiness_.specularGlossinessTexture_.index_,
//			material.data_.khrMaterialsSheen_.sheenColorTexture_.index_,
//			material.data_.khrMaterialsSheen_.sheenRoughnessTexture_.index_,
//			material.data_.khrMaterialsSpecular_.specularTexture_.index_,
//			material.data_.khrMaterialsSpecular_.specularColorTexture_.index_,
//			material.data_.khrMaterialsClearcoat_.clearcoatTexture_.index_,
//			material.data_.khrMaterialsClearcoat_.clearcoatRoughnessTexture_.index_,
//			material.data_.khrMaterialsClearcoat_.clearcoatNormalTexture_.index_,
//			material.data_.khrMaterialsTransmission_.transmissionTexture_.index_,
//			material.data_.khrMaterialsVolume_.thicknessTexture_.index_,
//		};
//		ID3D11ShaderResourceView* nullShaderResourceView = NULL;
//		std::vector<ID3D11ShaderResourceView*> shaderResourceViews(_countof(textureIndices_));
//		for (int textureIndex = 0; textureIndex < shaderResourceViews.size(); ++textureIndex)
//		{
//			shaderResourceViews.at(textureIndex) = textureIndices_[textureIndex] > -1 ? textureResourceViews_.at(textures_.at(textureIndices_[textureIndex]).source_).Get() : nullShaderResourceView;
//		}
//		deviceContext->PSSetShaderResources(1, static_cast<UINT>(shaderResourceViews.size()), shaderResourceViews.data());
//
//		primitiveConstants_->data_.color_ = { 1, 1, 1, 1 };
//		primitiveConstants_->data_.transform_ = transform;
//		primitiveConstants_->data_.material_ = primitive.material_ < 0 ? static_cast<int>(materials_.size() - 1) : primitive.material_;
//		primitiveConstants_->Activate(Primitive_Slot, CBufferUsage::vp);
//
//		deviceContext->IASetPrimitiveTopology(primitive.topology_);
//
//		drawcallCount++;
//		if (primitive.indexBufferView_.Count() > 0)
//		{
//			deviceContext->IASetIndexBuffer(buffers_.at(primitive.indexBufferView_.buffer_).Get(), primitive.indexBufferView_.format_, 0);
//			deviceContext->DrawIndexed(static_cast<UINT>(primitive.indexCount_), static_cast<UINT>(primitive.indexLocation_), 0);
//		}
//		else
//		{
//			deviceContext->Draw(static_cast<UINT>(primitive.vertexBufferViews_.at("POSITION").Count()), 0);
//		}
//	}
//	return drawcallCount;
//}
int GltfStaticMesh::Draw(DrawPass pass, const DirectX::XMFLOAT4X4& transform)
{
	ID3D11DeviceContext* deviceContext = DXSystem::GetDeviceContext();

	int drawcallCount = 0;

	deviceContext->VSSetShader(staticMeshVs_.Get(), nullptr, 0);
	deviceContext->PSSetShader(staticMeshPs_.Get(), nullptr, 0);
	deviceContext->IASetInputLayout(inputLayout_.Get());

	// The slot number of 'material_constants' are defined in 'material.hlsli'.
	const int materialConstantsSlot{ 0 };
	deviceContext->PSSetShaderResources(materialConstantsSlot, 1, materialResourceView_.GetAddressOf());

	for (decltype(batchPrimitives_)::const_reference primitive : batchPrimitives_)
	{
		if (primitive.vertexBufferViews_.at("POSITION").Count() == 0 || primitive.vertexBufferViews_.at("POSITION").buffer_ < 0)
		{
			continue;
		}


		const Material& material = materials_.at(primitive.material_ < 0 ? materials_.size() - 1 : primitive.material_);
		_ASSERT_EXPR(material.data_.normalTexture_.index_ > -1 ? primitive.vertexBufferViews_.at("TANGENT").buffer_ > -1 : TRUE, L"");

#if 1
		bool permitted = false;
		switch (pass)
		{
		case DrawPass::Opaque:
			permitted = material.HasOpaque();
			break;
		case DrawPass::Transmission:
			permitted = material.HasTransmission();
			break;
		}
		if (!permitted)
		{
			continue;
		}
#endif

		ID3D11Buffer* vertexBuffers[] =
		{
			primitive.vertexBufferViews_.at("POSITION").buffer_ > -1 ? buffers_.at(primitive.vertexBufferViews_.at("POSITION").buffer_).Get() : NULL,
			primitive.vertexBufferViews_.at("NORMAL").buffer_ > -1 ? buffers_.at(primitive.vertexBufferViews_.at("NORMAL").buffer_).Get() : NULL,
			primitive.vertexBufferViews_.at("TANGENT").buffer_ > -1 ? buffers_.at(primitive.vertexBufferViews_.at("TANGENT").buffer_).Get() : NULL,
			primitive.vertexBufferViews_.at("TEXCOORD_0").buffer_ > -1 ? buffers_.at(primitive.vertexBufferViews_.at("TEXCOORD_0").buffer_).Get() : NULL,
			primitive.vertexBufferViews_.at("TEXCOORD_1").buffer_ > -1 ? buffers_.at(primitive.vertexBufferViews_.at("TEXCOORD_1").buffer_).Get() : NULL,
		};
		UINT strides[] =
		{
			static_cast<UINT>(primitive.vertexBufferViews_.at("POSITION").strideInBytes_),
			static_cast<UINT>(primitive.vertexBufferViews_.at("NORMAL").strideInBytes_),
			static_cast<UINT>(primitive.vertexBufferViews_.at("TANGENT").strideInBytes_),
			static_cast<UINT>(primitive.vertexBufferViews_.at("TEXCOORD_0").strideInBytes_),
			static_cast<UINT>(primitive.vertexBufferViews_.at("TEXCOORD_1").strideInBytes_),
		};

		UINT offsets[_countof(vertexBuffers)] = { 0 };
		deviceContext->IASetVertexBuffers(0, _countof(vertexBuffers), vertexBuffers, strides, offsets);

		// The slot numbers of 'primitive_textures' are defined in 'material.hlsli'.
		const int textureIndices_[] =
		{
			material.data_.pbrMetallicRoughness_.basecolorTexture_.index_,
			material.data_.pbrMetallicRoughness_.metallicRoughnessTexture_.index_,
			material.data_.normalTexture_.index_,
			material.data_.emissiveTexture_.index_,
			material.data_.occlusionTexture_.index_,
			material.data_.KhrMaterialsPbrSpecularGlossiness_.diffuseTexture_.index_,
			material.data_.KhrMaterialsPbrSpecularGlossiness_.specularGlossinessTexture_.index_,
			material.data_.khrMaterialsSheen_.sheenColorTexture_.index_,
			material.data_.khrMaterialsSheen_.sheenRoughnessTexture_.index_,
			material.data_.khrMaterialsSpecular_.specularTexture_.index_,
			material.data_.khrMaterialsSpecular_.specularColorTexture_.index_,
			material.data_.khrMaterialsClearcoat_.clearcoatTexture_.index_,
			material.data_.khrMaterialsClearcoat_.clearcoatRoughnessTexture_.index_,
			material.data_.khrMaterialsClearcoat_.clearcoatNormalTexture_.index_,
			material.data_.khrMaterialsTransmission_.transmissionTexture_.index_,
			material.data_.khrMaterialsVolume_.thicknessTexture_.index_,
		};

		ID3D11ShaderResourceView* nullShaderResourceView = NULL;
		std::vector<ID3D11ShaderResourceView*> shaderResourceViews(_countof(textureIndices_));
		for (int texture_index = 0; texture_index < shaderResourceViews.size(); ++texture_index)
		{
			shaderResourceViews.at(texture_index) = textureIndices_[texture_index] > -1 ? textureResourceViews_.at(textures_.at(textureIndices_[texture_index]).source_).Get() : nullShaderResourceView;
		}
		deviceContext->PSSetShaderResources(1, static_cast<UINT>(shaderResourceViews.size()), shaderResourceViews.data());

		primitiveConstants_->data_.color_ = { 1, 1, 1, 1 };
		primitiveConstants_->data_.transform_ = transform;
		primitiveConstants_->data_.material_ = primitive.material_ < 0 ? static_cast<int>(materials_.size() - 1) : primitive.material_;
		primitiveConstants_->Activate(Primitive_Slot, CBufferUsage::vp);

		deviceContext->IASetPrimitiveTopology(primitive.topology_);

		drawcallCount++;
		if (primitive.indexBufferView_.Count() > 0)
		{
			deviceContext->IASetIndexBuffer(buffers_.at(primitive.indexBufferView_.buffer_).Get(), primitive.indexBufferView_.format_, 0);
			deviceContext->DrawIndexed(static_cast<UINT>(primitive.indexCount_), static_cast<UINT>(primitive.indexLocation_), 0);
		}
		else
		{
			deviceContext->Draw(static_cast<UINT>(primitive.vertexBufferViews_.at("POSITION").Count()), 0);
		}
	}
	return drawcallCount;
}
int GltfStaticMesh::CastShadow(const DirectX::XMFLOAT4X4& transform)
{
	ID3D11DeviceContext* deviceContext = DXSystem::GetDeviceContext();

	int drawcall_count = 0;

	for (int pass = 0; pass < 2; ++pass)
	{
		for (decltype(batchPrimitives_)::const_reference primitive : batchPrimitives_)
		{
			const Material& material = materials_.at(primitive.material_ < 0 ? materials_.size() - 1 : primitive.material_);

			bool permitted = false;
			switch (pass)
			{
			case 0: // opaque
				permitted = material.HasOpaque();
				break;
			case 1: // transmission
				permitted = material.HasTransmission();
				break;
			}
			if (!permitted)
			{
				continue;
			}

			if (pass == 0 /*opaque*/)
			{
				deviceContext->VSSetShader(csmOpaqueStaticMeshVs_.Get(), NULL, 0);
				deviceContext->GSSetShader(csmOpaqueGs_.Get(), NULL, 0);
				deviceContext->PSSetShader(NULL, NULL, 0);
				deviceContext->IASetInputLayout(csmOpaquInputLayout_.Get());

				ID3D11Buffer* vertex_buffers[] =
				{
					buffers_.at(primitive.vertexBufferViews_.at("POSITION").buffer_).Get(),
					NULL,
				};
				UINT strides[] =
				{
					static_cast<UINT>(primitive.vertexBufferViews_.at("POSITION").strideInBytes_),
					0,
				};
				UINT offsets[_countof(vertex_buffers)] = { 0 };
				deviceContext->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
			}
			else /*transparent*/
			{
				deviceContext->VSSetShader(csmTransparentStaticMeshVs_.Get(), NULL, 0);
				deviceContext->GSSetShader(csmTransparentGs_.Get(), NULL, 0);
				deviceContext->PSSetShader(csmTransparentPs_.Get(), NULL, 0);
				deviceContext->IASetInputLayout(csmTransparentInputLayout_.Get());

				ID3D11Buffer* vertexBuffers[] =
				{
					buffers_.at(primitive.vertexBufferViews_.at("POSITION").buffer_).Get(),
					NULL,
				};
				UINT strides[] =
				{
					static_cast<UINT>(primitive.vertexBufferViews_.at("POSITION").strideInBytes_),
					0,
				};
				int texcoord = material.data_.pbrMetallicRoughness_.basecolorTexture_.texcoord_;
				switch (texcoord)
				{
				case 0:
					vertexBuffers[1] = buffers_.at(primitive.vertexBufferViews_.at("TEXCOORD_0").buffer_).Get();
					strides[1] = static_cast<UINT>(primitive.vertexBufferViews_.at("TEXCOORD_0").strideInBytes_);
					break;
				case 1:
					vertexBuffers[1] = buffers_.at(primitive.vertexBufferViews_.at("TEXCOORD_1").buffer_).Get();
					strides[1] = static_cast<UINT>(primitive.vertexBufferViews_.at("TEXCOORD_1").strideInBytes_);
					break;
				}
				UINT offsets[_countof(vertexBuffers)] = { 0 };
				deviceContext->IASetVertexBuffers(0, _countof(vertexBuffers), vertexBuffers, strides, offsets);

				ID3D11ShaderResourceView* textureResourceView = NULL;
				int basecolorTextureIndex = material.data_.pbrMetallicRoughness_.basecolorTexture_.index_;
				if (basecolorTextureIndex > -1)
				{
					int image = textures_.at(basecolorTextureIndex).source_;
					if (image > -1)
					{
						textureResourceView = textureResourceViews_.at(image).Get();
						deviceContext->PSSetShaderResources(1, 1, &textureResourceView);
					}
				}
			}

#if 1
			if (primitive.vertexBufferViews_.at("POSITION").buffer_ < 0)
			{
				continue;
			}
#endif

			primitiveConstants_->data_.transform_ = transform;
			primitiveConstants_->data_.startInstanceLocation_ = 0;
			primitiveConstants_->data_.skin_ = -1;
			primitiveConstants_->Activate(Primitive_Slot, CBufferUsage::v);

			deviceContext->IASetPrimitiveTopology(primitive.topology_);

			drawcall_count++;
			if (primitive.indexBufferView_.Count() > 0)
			{
				deviceContext->IASetIndexBuffer(buffers_.at(primitive.indexBufferView_.buffer_).Get(), primitive.indexBufferView_.format_, 0);
				deviceContext->DrawIndexedInstanced(static_cast<UINT>(primitive.indexCount_), 4, static_cast<UINT>(primitive.indexLocation_), 0, 0);
			}
			else
			{
				deviceContext->DrawInstanced(static_cast<UINT>(primitive.vertexBufferViews_.at("POSITION").Count()), 4, 0, 0);
			}
		}
	}

	deviceContext->VSSetShader(NULL, NULL, 0);
	deviceContext->PSSetShader(NULL, NULL, 0);
	deviceContext->GSSetShader(NULL, NULL, 0);
	deviceContext->IASetInputLayout(NULL);

	return drawcall_count;
}
