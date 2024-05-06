#include "GltfSkeletalMesh.h"

#include <sstream>
#include <functional>

#include "Misc.h"
#include "Shader.h"
#include "Texture.h"
#include "BoundingBox.h"

using namespace AbyssEngine;
using namespace DirectX;

GltfSkeletalMesh::GltfSkeletalMesh(const std::string& filename) : GeometricSubstance(filename)
{
	HRESULT hr = S_OK;

	tinygltf::TinyGLTF tinyGltf;
	tinyGltf.SetImageLoader(NullLoadImageData, nullptr);

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
	ExtractAnimations(transmissionModel);

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "JOINTS", 0, DXGI_FORMAT_R16G16B16A16_UINT, 5, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0,DXGI_FORMAT_R32G32B32A32_FLOAT, 6, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	skeletalMeshVs_ = Shader<ID3D11VertexShader>::Emplace("skeletal_mesh_vs.cso",inputLayout_.ReleaseAndGetAddressOf(), inputElementDesc, _countof(inputElementDesc));
	skeletalMeshPs_ = Shader<ID3D11PixelShader>::Emplace("GeometricSubstance_ps.cso");

	D3D11_INPUT_ELEMENT_DESC csmOpaqueInputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "JOINTS", 0, DXGI_FORMAT_R16G16B16A16_UINT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0,DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	csmOpaqueSkeletalMeshVs_	= Shader<ID3D11VertexShader>::Emplace("csm_opaque_skeletal_mesh_vs.cso",csmOpaqueInputLayout_.ReleaseAndGetAddressOf(), csmOpaqueInputElementDesc, _countof(csmOpaqueInputElementDesc));
	csmOpaqueGs_				= Shader<ID3D11GeometryShader>::Emplace("csm_opaque_gs.cso");

	D3D11_INPUT_ELEMENT_DESC csmTransparentInputElementDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "JOINTS", 0, DXGI_FORMAT_R16G16B16A16_UINT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0,DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	csmTransparentSkeletalMeshVs_	= Shader<ID3D11VertexShader>::Emplace("csm_transparent_skeletal_mesh_vs.cso",csm_transparent_input_layout.ReleaseAndGetAddressOf(), csmTransparentInputElementDesc, _countof(csmTransparentInputElementDesc));
	csmTransparentGs_				= Shader<ID3D11GeometryShader>::Emplace("csm_transparent_gs.cso");
	csmTransparentPs_				= Shader<ID3D11PixelShader>::Emplace("csm_transparent_ps.cso");

	jointConstants_ = std::make_unique<decltype(jointConstants_)::element_type>();
}
void GltfSkeletalMesh::ExtractMeshes(const tinygltf::Model& transmissionModel)
{
	ID3D11Device* device = DXSystem::device_.Get();

	HRESULT hr{ S_OK };

	std::function<BufferView(const tinygltf::Accessor&)> makeBufferView = [](const tinygltf::Accessor& accessor)->BufferView
	{
		BufferView bufferView;
		switch (accessor.type)
		{
		case TINYGLTF_TYPE_SCALAR:
			switch (accessor.componentType)
			{
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
				bufferView.format_ = DXGI_FORMAT_R16_UINT;
				bufferView.strideInBytes_ = sizeof(USHORT);
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				bufferView.format_ = DXGI_FORMAT_R32_UINT;
				bufferView.strideInBytes_ = sizeof(UINT);
				break;
			default:
				_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
				break;
			}
			break;
		case TINYGLTF_TYPE_VEC2:
			switch (accessor.componentType)
			{
			case TINYGLTF_COMPONENT_TYPE_BYTE:
				bufferView.format_ = DXGI_FORMAT_R8G8_SINT;
				bufferView.strideInBytes_ = sizeof(CHAR) * 2;
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
				bufferView.format_ = DXGI_FORMAT_R8G8_UINT;
				bufferView.strideInBytes_ = sizeof(BYTE) * 2;
				break;
			case TINYGLTF_COMPONENT_TYPE_SHORT:
				bufferView.format_ = DXGI_FORMAT_R16G16_SINT;
				bufferView.strideInBytes_ = sizeof(SHORT) * 2;
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
				bufferView.format_ = DXGI_FORMAT_R16G16_UINT;
				bufferView.strideInBytes_ = sizeof(USHORT) * 2;
				break;
			case TINYGLTF_COMPONENT_TYPE_INT:
				bufferView.format_ = DXGI_FORMAT_R32G32_SINT;
				bufferView.strideInBytes_ = sizeof(INT) * 2;
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				bufferView.format_ = DXGI_FORMAT_R32G32_UINT;
				bufferView.strideInBytes_ = sizeof(UINT) * 2;
				break;
			case TINYGLTF_COMPONENT_TYPE_FLOAT:
				bufferView.format_ = DXGI_FORMAT_R32G32_FLOAT;
				bufferView.strideInBytes_ = sizeof(FLOAT) * 2;
				break;
			case TINYGLTF_COMPONENT_TYPE_DOUBLE:
			default:
				_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
				break;
			}
			break;
		case TINYGLTF_TYPE_VEC3:
			switch (accessor.componentType)
			{
			case TINYGLTF_COMPONENT_TYPE_INT:
				bufferView.format_ = DXGI_FORMAT_R32G32B32_SINT;
				bufferView.strideInBytes_ = sizeof(INT) * 3;
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				bufferView.format_ = DXGI_FORMAT_R32G32B32_UINT;
				bufferView.strideInBytes_ = sizeof(UINT) * 3;
				break;
			case TINYGLTF_COMPONENT_TYPE_FLOAT:
				bufferView.format_ = DXGI_FORMAT_R32G32B32_FLOAT;
				bufferView.strideInBytes_ = sizeof(FLOAT) * 3;
				break;
			case TINYGLTF_COMPONENT_TYPE_BYTE:
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			case TINYGLTF_COMPONENT_TYPE_SHORT:
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			case TINYGLTF_COMPONENT_TYPE_DOUBLE:
			default:
				_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
				break;
			}
			break;
		case TINYGLTF_TYPE_VEC4:
			switch (accessor.componentType)
			{
			case TINYGLTF_COMPONENT_TYPE_BYTE:
				bufferView.format_ = DXGI_FORMAT_R8G8B8A8_SINT;
				bufferView.strideInBytes_ = sizeof(CHAR) * 4;
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
				bufferView.format_ = DXGI_FORMAT_R8G8B8A8_UINT;
				bufferView.strideInBytes_ = sizeof(BYTE) * 4;
				break;
			case TINYGLTF_COMPONENT_TYPE_SHORT:
				bufferView.format_ = DXGI_FORMAT_R16G16B16A16_SINT;
				bufferView.strideInBytes_ = sizeof(SHORT) * 4;
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
				bufferView.format_ = DXGI_FORMAT_R16G16B16A16_UINT;
				bufferView.strideInBytes_ = sizeof(USHORT) * 4;
				break;
			case TINYGLTF_COMPONENT_TYPE_INT:
				bufferView.format_ = DXGI_FORMAT_R32G32B32A32_SINT;
				bufferView.strideInBytes_ = sizeof(INT) * 4;
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				bufferView.format_ = DXGI_FORMAT_R32G32B32A32_UINT;
				bufferView.strideInBytes_ = sizeof(UINT) * 4;
				break;
			case TINYGLTF_COMPONENT_TYPE_FLOAT:
				bufferView.format_ = DXGI_FORMAT_R32G32B32A32_FLOAT;
				bufferView.strideInBytes_ = sizeof(FLOAT) * 4;
				break;
			case TINYGLTF_COMPONENT_TYPE_DOUBLE:
			default:
				_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
				break;
			}
			break;
		default:
			_ASSERT_EXPR(FALSE, L"This accessor type is not supported.");
			break;
		}
		bufferView.sizeInBytes_ = accessor.count * bufferView.strideInBytes_;
		return bufferView;
	};

	for (std::vector<tinygltf::Mesh>::const_reference transmissionMesh : transmissionModel.meshes)
	{
		Mesh& mesh = meshes_.emplace_back();
		mesh.name_ = transmissionMesh.name;

		for (std::vector<tinygltf::Primitive>::const_reference transmissionPrimitive : transmissionMesh.primitives)
		{
			Primitive& primitive = mesh.primitives_.emplace_back();
			primitive.material_ = transmissionPrimitive.material;
			const std::unordered_map<size_t, D3D_PRIMITIVE_TOPOLOGY> topologies =
			{
				{ TINYGLTF_MODE_TRIANGLES, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST },
				{ TINYGLTF_MODE_TRIANGLE_STRIP, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP },
				{ TINYGLTF_MODE_POINTS, D3D_PRIMITIVE_TOPOLOGY_POINTLIST },
				{ TINYGLTF_MODE_LINE, D3D_PRIMITIVE_TOPOLOGY_LINELIST },
				{ TINYGLTF_MODE_LINE_STRIP, D3D_PRIMITIVE_TOPOLOGY_LINESTRIP },
			};
			primitive.topology_ = topologies.at(transmissionPrimitive.mode);

			// Create index buffer
			if (transmissionPrimitive.indices > -1)
			{
				tinygltf::Accessor transmissionAccessor = transmissionModel.accessors.at(transmissionPrimitive.indices);
				const tinygltf::BufferView& transmissionBufferView = transmissionModel.bufferViews.at(transmissionAccessor.bufferView);

				const unsigned char* buffer = transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset;
				std::vector<unsigned short> extraIndices; // Use for TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE formatted index buffer
				if (transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
				{
					// Convert to TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT
					extraIndices.resize(transmissionAccessor.count);
					for (size_t i = 0; i < transmissionAccessor.count; ++i)
					{
						extraIndices.at(i) = static_cast<unsigned short>(buffer[i]);
					}
					transmissionAccessor.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT;
					buffer = reinterpret_cast<const unsigned char*>(extraIndices.data());
				}
				primitive.indexBufferView_ = makeBufferView(transmissionAccessor);
				primitive.indexBufferView_.buffer_ = static_cast<int>(buffers_.size());
				primitive.indexLocation_ = 0;
				primitive.indexCount_ = transmissionAccessor.count;
				
				D3D11_BUFFER_DESC bufferDesc = {};
				D3D11_SUBRESOURCE_DATA subresourceData = {};
				bufferDesc.ByteWidth = static_cast<UINT>(primitive.indexBufferView_.sizeInBytes_);
				bufferDesc.Usage = D3D11_USAGE_DEFAULT;
				bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				bufferDesc.CPUAccessFlags = 0;
				bufferDesc.MiscFlags = 0;
				bufferDesc.StructureByteStride = 0;
				subresourceData.pSysMem = buffer;
				subresourceData.SysMemPitch = 0;
				subresourceData.SysMemSlicePitch = 0;
				hr = device->CreateBuffer(&bufferDesc, &subresourceData, buffers_.emplace_back().ReleaseAndGetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
			}
			// Create vertex buffers
			for (std::map<std::string, int>::const_reference transmissionAttribute : transmissionPrimitive.attributes)
			{
				tinygltf::Accessor transmissionAccessor = transmissionModel.accessors.at(transmissionAttribute.second);
				const tinygltf::BufferView& transmissionBufferView = transmissionModel.bufferViews.at(transmissionAccessor.bufferView);

				bool supportedAttribute = true;

				const unsigned char* buffer = transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset;

				std::vector<unsigned short> extraJoints;
				std::vector<float> extraWeights;

				// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#buffers-and-buffer-views
				if (transmissionAttribute.first == "POSITION")
				{
					// Constructing a bounding box
					if (transmissionAccessor.maxValues.size() == 3 && transmissionAccessor.minValues.size() == 3)
					{
						primitive.maxValue_ = { static_cast<float>(transmissionAccessor.maxValues.at(0)), static_cast<float>(transmissionAccessor.maxValues.at(1)), static_cast<float>(transmissionAccessor.maxValues.at(2)) };
						primitive.minValue_ = { static_cast<float>(transmissionAccessor.minValues.at(0)), static_cast<float>(transmissionAccessor.minValues.at(1)), static_cast<float>(transmissionAccessor.minValues.at(2)) };
					}
				}
				else if (transmissionAttribute.first == "NORMAL")
				{
				}
				else if (transmissionAttribute.first == "TANGENT")
				{
				}
				else if (transmissionAttribute.first == "TEXCOORD_0")
				{
					_ASSERT_EXPR(transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && transmissionAccessor.type == TINYGLTF_TYPE_VEC2, L"'TEXCOORD_0' attribute must be of type TINYGLTF_COMPONENT_TYPE_FLOAT & TINYGLTF_TYPE_VEC2.");
				}
				else if (transmissionAttribute.first == "TEXCOORD_1")
				{
					_ASSERT_EXPR(transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && transmissionAccessor.type == TINYGLTF_TYPE_VEC2, L"'TEXCOORD_0' attribute must be of type TINYGLTF_COMPONENT_TYPE_FLOAT & TINYGLTF_TYPE_VEC2.");
				}
				else if (transmissionAttribute.first == "JOINTS_0")
				{
					_ASSERT_EXPR(transmissionAccessor.type == TINYGLTF_TYPE_VEC4, L"'JOINTS_0' attribute must be of TINYGLTF_TYPE_VEC4.");
					if (transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT || transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_SHORT)
					{
						transmissionAccessor.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT;
					}
					else if (transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE || transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_BYTE)
					{
						// Convert to TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT
						extraJoints.resize(transmissionAccessor.count * 4);
						for (size_t i = 0; i < transmissionAccessor.count * 4; ++i)
						{
							extraJoints.at(i) = static_cast<unsigned short>(buffer[i]);
						}
						transmissionAccessor.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT;
						buffer = reinterpret_cast<unsigned char*>(extraJoints.data());
					}
					else
					{
						_ASSERT_EXPR(FALSE, L"This TINYGLTF_COMPONENT_TYPE of JOINTS_0 is not supported");
					}
				}
				else if (transmissionAttribute.first == "WEIGHTS_0")
				{
					_ASSERT_EXPR(transmissionAccessor.type == TINYGLTF_TYPE_VEC4, L"'JOINTS_0' attribute must be of TINYGLTF_TYPE_VEC4.");
					if (transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
					{
					}
					else if (transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
					{
						// Convert to TINYGLTF_COMPONENT_TYPE_FLOAT
						extraWeights.resize(transmissionAccessor.count * 4);
						for (size_t accessor_index = 0; accessor_index < transmissionAccessor.count; ++accessor_index)
						{
							extraWeights.at(accessor_index * 4 + 0) = static_cast<float>(buffer[accessor_index * 4 + 0]) / 0xFF;
							extraWeights.at(accessor_index * 4 + 1) = static_cast<float>(buffer[accessor_index * 4 + 1]) / 0xFF;
							extraWeights.at(accessor_index * 4 + 2) = static_cast<float>(buffer[accessor_index * 4 + 2]) / 0xFF;
							extraWeights.at(accessor_index * 4 + 3) = static_cast<float>(buffer[accessor_index * 4 + 3]) / 0xFF;
						}
						transmissionAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
						buffer = reinterpret_cast<unsigned char*>(extraWeights.data());
					}
					else if (transmissionAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
					{
						// Convert to TINYGLTF_COMPONENT_TYPE_FLOAT
						extraWeights.resize(transmissionAccessor.count * 4);
						for (size_t accessor_index = 0; accessor_index < transmissionAccessor.count; ++accessor_index)
						{
							extraWeights.at(accessor_index * 4 + 0) = static_cast<float>(reinterpret_cast<const unsigned short*>(buffer)[accessor_index * 4 + 0]) / 0xFFFF;
							extraWeights.at(accessor_index * 4 + 1) = static_cast<float>(reinterpret_cast<const unsigned short*>(buffer)[accessor_index * 4 + 1]) / 0xFFFF;
							extraWeights.at(accessor_index * 4 + 2) = static_cast<float>(reinterpret_cast<const unsigned short*>(buffer)[accessor_index * 4 + 2]) / 0xFFFF;
							extraWeights.at(accessor_index * 4 + 3) = static_cast<float>(reinterpret_cast<const unsigned short*>(buffer)[accessor_index * 4 + 3]) / 0xFFFF;
						}
						transmissionAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
						buffer = reinterpret_cast<unsigned char*>(extraWeights.data());
					}
					else
					{
						_ASSERT_EXPR(FALSE, L"This TINYGLTF_COMPONENT_TYPE of WEIGHTS_0 is not supported");
					}
				}
				else if (transmissionAttribute.first == "COLOR_0")
				{
					supportedAttribute = false;
					OutputDebugStringA(std::string("We do not support vertex attribute : " + transmissionAttribute.first + '\n').c_str());
				}
				else
				{
					supportedAttribute = false;
					OutputDebugStringA(std::string("We do not support vertex attribute : " + transmissionAttribute.first + '\n').c_str());
				}

				if (supportedAttribute)
				{
					BufferView vertexBufferView = makeBufferView(transmissionAccessor);
					vertexBufferView.buffer_ = static_cast<int>(buffers_.size());

					D3D11_BUFFER_DESC bufferDesc{};
					D3D11_SUBRESOURCE_DATA subresourceData{};
					bufferDesc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
					bufferDesc.Usage = D3D11_USAGE_DEFAULT;
					bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					bufferDesc.CPUAccessFlags = 0;
					bufferDesc.MiscFlags = 0;
					bufferDesc.StructureByteStride = 0;
					subresourceData.pSysMem = buffer;
					subresourceData.SysMemPitch = 0;
					subresourceData.SysMemSlicePitch = 0;
					hr = device->CreateBuffer(&bufferDesc, &subresourceData, buffers_.emplace_back().ReleaseAndGetAddressOf());
					_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

					primitive.vertexBufferViews_.emplace(std::make_pair(transmissionAttribute.first, vertexBufferView));
				}
			}

#if 1
			std::function<std::tuple<const void*, size_t>(const std::string& attribute)> addressof = [&](const std::string& attribute)->std::tuple<const void*, size_t>
			{
				_ASSERT_EXPR(transmissionPrimitive.attributes.find(attribute) != transmissionPrimitive.attributes.end(), L"");

				std::tuple<const void*, size_t> tupleValue;

				std::vector<tinygltf::Accessor>::const_reference transmissionAccessor = transmissionModel.accessors.at(transmissionPrimitive.attributes.at(attribute));
				std::vector<tinygltf::BufferView>::const_reference transmissionBufferView = transmissionModel.bufferViews.at(transmissionAccessor.bufferView);

				std::get<0>(tupleValue) = transmissionModel.buffers.at(transmissionBufferView.buffer).data.data() + transmissionBufferView.byteOffset + transmissionAccessor.byteOffset;
				std::get<1>(tupleValue) = transmissionAccessor.count;

				return tupleValue;
			};
			// If this primitive does not have normals, calculate them.
			std::vector<DirectX::XMFLOAT3> extraNormals;
			if (primitive.vertexBufferViews_.find("NORMAL") == primitive.vertexBufferViews_.end())
			{
				std::tuple<const void*, size_t> tupleValue = addressof("POSITION");
				const DirectX::XMFLOAT3* positions = reinterpret_cast<const DirectX::XMFLOAT3*>(std::get<0>(tupleValue));
				const size_t vertexCount = std::get<1>(tupleValue);

				extraNormals.resize(vertexCount);
				DirectX::XMFLOAT3* normals = extraNormals.data();

				_ASSERT_EXPR(transmissionPrimitive.indices > -1, L"An index buffer is needed for this process.");
				std::vector<tinygltf::Accessor>::const_reference transmission_accessor = transmissionModel.accessors.at(transmissionPrimitive.indices);
				std::vector<tinygltf::BufferView>::const_reference transmission_buffer_view = transmissionModel.bufferViews.at(transmission_accessor.bufferView);
				const void* indices = transmissionModel.buffers.at(transmission_buffer_view.buffer).data.data() + transmission_buffer_view.byteOffset + transmission_accessor.byteOffset;
				size_t index_count = transmission_accessor.count;

				if (transmissionModel.accessors.at(transmissionPrimitive.indices).componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
				{
					ComputeNormals<unsigned int>(positions, vertexCount, reinterpret_cast<const unsigned int*>(indices), index_count, normals);
				}
				else if (transmissionModel.accessors.at(transmissionPrimitive.indices).componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
				{
					ComputeNormals<unsigned short>(positions, vertexCount, reinterpret_cast<const unsigned short*>(indices), index_count, normals);
				}
				else
				{
					_ASSERT_EXPR(FALSE, L"This index format is not supported.");
				}

				BufferView vertexBufferView = {};
				vertexBufferView.format_ = DXGI_FORMAT_R32G32B32_FLOAT;
				vertexBufferView.strideInBytes_ = sizeof(DirectX::XMFLOAT3);
				vertexBufferView.sizeInBytes_ = vertexCount * vertexBufferView.strideInBytes_;
				vertexBufferView.buffer_ = static_cast<int>(buffers_.size());

				D3D11_BUFFER_DESC bufferDesc = {};
				D3D11_SUBRESOURCE_DATA subresourceData = {};
				bufferDesc.ByteWidth = static_cast<UINT>(vertexBufferView.strideInBytes_);
				bufferDesc.Usage = D3D11_USAGE_DEFAULT;
				bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				bufferDesc.CPUAccessFlags = 0;
				bufferDesc.MiscFlags = 0;
				bufferDesc.StructureByteStride = 0;
				subresourceData.pSysMem = normals;
				subresourceData.SysMemPitch = 0;
				subresourceData.SysMemSlicePitch = 0;
				hr = device->CreateBuffer(&bufferDesc, &subresourceData, buffers_.emplace_back().ReleaseAndGetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

				primitive.vertexBufferViews_.insert(std::make_pair("NORMAL", vertexBufferView));
			}
			// If this primitive does not have tangents, calculate them.
			const bool hasNormal = materials_.at(primitive.material_).data_.normalTexture_.index_ > -1;
			if (hasNormal && primitive.vertexBufferViews_.find("TANGENT") == primitive.vertexBufferViews_.end() && primitive.vertexBufferViews_.find("TEXCOORD_0") != primitive.vertexBufferViews_.end())
			{
				std::tuple<const void*, size_t> tuple_value;

				tuple_value = addressof("POSITION");
				const DirectX::XMFLOAT3* positions = reinterpret_cast<const DirectX::XMFLOAT3*>(std::get<0>(tuple_value));
				const size_t vertex_count = std::get<1>(tuple_value);

				const DirectX::XMFLOAT3* normals = {};
				if (transmissionPrimitive.attributes.find("NORMAL") == transmissionPrimitive.attributes.end())
				{
					_ASSERT_EXPR(extraNormals.size() > 0, L"");
					normals = extraNormals.data();
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
				std::vector<tinygltf::Accessor>::const_reference transmission_accessor{ transmissionModel.accessors.at(transmissionPrimitive.indices) };
				std::vector<tinygltf::BufferView>::const_reference transmission_buffer_view{ transmissionModel.bufferViews.at(transmission_accessor.bufferView) };
				const void* indices = transmissionModel.buffers.at(transmission_buffer_view.buffer).data.data() + transmission_buffer_view.byteOffset + transmission_accessor.byteOffset;
				size_t indexCount = transmission_accessor.count;

				std::vector<DirectX::XMFLOAT4> tangents;
				tangents.resize(vertex_count);
				if (transmissionModel.accessors.at(transmissionPrimitive.indices).componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
				{
					ComputeTangents<unsigned int>(positions, normals, texcoords, vertex_count, reinterpret_cast<const unsigned int*>(indices), indexCount, tangents.data());
				}
				else if (transmissionModel.accessors.at(transmissionPrimitive.indices).componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
				{
					ComputeTangents<unsigned short>(positions, normals, texcoords, vertex_count, reinterpret_cast<const unsigned short*>(indices), indexCount, tangents.data());
				}
				else
				{
					_ASSERT_EXPR(FALSE, L"This index format is not supported.");
				}

				BufferView vertexBufferView;
				vertexBufferView.format_ = DXGI_FORMAT_R32G32B32A32_FLOAT;
				vertexBufferView.strideInBytes_ = sizeof(DirectX::XMFLOAT4);
				vertexBufferView.sizeInBytes_ = vertex_count * vertexBufferView.strideInBytes_;
				vertexBufferView.buffer_ = static_cast<int>(buffers_.size());

				D3D11_BUFFER_DESC bufferDesc = {};
				D3D11_SUBRESOURCE_DATA subresourceData = {};
				bufferDesc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
				bufferDesc.Usage = D3D11_USAGE_DEFAULT;
				bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				bufferDesc.CPUAccessFlags = 0;
				bufferDesc.MiscFlags = 0;
				bufferDesc.StructureByteStride = 0;
				subresourceData.pSysMem = tangents.data();
				subresourceData.SysMemPitch = 0;
				subresourceData.SysMemSlicePitch = 0;
				hr = device->CreateBuffer(&bufferDesc, &subresourceData, buffers_.emplace_back().ReleaseAndGetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

				primitive.vertexBufferViews_.insert(std::make_pair("TANGENT", vertexBufferView));
			}
#endif

#if 1
			_ASSERT_EXPR(primitive.vertexBufferViews_.find("POSITION") != primitive.vertexBufferViews_.end(), L"POSITION is required for our vertex attributes.");
#endif
			// Add dummy attributes if any are missing.
			const std::unordered_map<std::string, BufferView> attributes =
			{
				{ "POSITION", { DXGI_FORMAT_R32G32B32_FLOAT } },
				{ "NORMAL", { DXGI_FORMAT_R32G32B32_FLOAT } },
				{ "TANGENT", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
				{ "TEXCOORD_0", { DXGI_FORMAT_R32G32_FLOAT } },
				{ "TEXCOORD_1", { DXGI_FORMAT_R32G32_FLOAT } },
				{ "JOINTS_0", { DXGI_FORMAT_R16G16B16A16_UINT } },
				{ "WEIGHTS_0", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
			};
			for (std::unordered_map<std::string, BufferView>::const_reference attribute : attributes)
			{
				if (primitive.vertexBufferViews_.find(attribute.first) == primitive.vertexBufferViews_.end())
				{
					primitive.vertexBufferViews_.insert(std::make_pair(attribute.first, attribute.second));
				}
			}
		}
	}

	// Rebuild bounding boxes (min_value / man_value) computationally.
	for (decltype(nodes_)::reference node : nodes_)
	{
		if (node.mesh_ > -1)
		{
			const Mesh& mesh = meshes_.at(node.mesh_);

			XMMATRIX transform = DirectX::XMLoadFloat4x4(&node.globalTransform_);
			for (std::vector<Primitive>::const_reference primitive : mesh.primitives_)
			{
				XMVECTOR maxValue = XMLoadFloat3(&primitive.maxValue_);
				XMVECTOR minValue = XMLoadFloat3(&primitive.minValue_);
				ComputeTransformedBounds(minValue, maxValue, transform);

				XMStoreFloat3(&node.maxValue_, XMVectorMax(XMLoadFloat3(&node.maxValue_), maxValue));
				XMStoreFloat3(&node.minValue_, XMVectorMin(XMLoadFloat3(&node.minValue_), minValue));
			}
			DirectX::XMStoreFloat3(&maxValue_, XMVectorMax(XMLoadFloat3(&maxValue_), XMLoadFloat3(&node.maxValue_)));
			DirectX::XMStoreFloat3(&minValue_, XMVectorMin(XMLoadFloat3(&minValue_), XMLoadFloat3(&node.minValue_)));
		}
	}
}

int GltfSkeletalMesh::Draw(DrawPass pass, const DirectX::XMFLOAT4X4& transform, const DirectX::XMFLOAT4& color, size_t sceneIndex)
{
	return Draw(pass, transform, nodes_, color, sceneIndex);
}
int GltfSkeletalMesh::Draw(DrawPass pass, const DirectX::XMFLOAT4X4& transform, const std::vector<Node>& animatedNodes, const DirectX::XMFLOAT4& color, size_t sceneIndex)
{
	ID3D11DeviceContext* deviceContext = DXSystem::deviceContext_.Get();

	_ASSERT_EXPR(animatedNodes.size() == nodes_.size(), L"");

	int drawcallCount = 0;

	using namespace DirectX;
	XMMATRIX worldTransform = XMLoadFloat4x4(&transform);

	deviceContext->VSSetShader(skeletalMeshVs_.Get(), nullptr, 0);
	deviceContext->PSSetShader(skeletalMeshPs_.Get(), nullptr, 0);
	deviceContext->IASetInputLayout(inputLayout_.Get());

	// The slot number of 'material_constants' are defined in 'material.hlsli'.
	const int material_constants_slot = 0;
	deviceContext->PSSetShaderResources(material_constants_slot, 1, materialResourceView_.GetAddressOf());

	std::function<void(int)> traverse = [&](int nodeIndex) {
		const Node& node = animatedNodes.at(nodeIndex);
		XMMATRIX globalTransform = XMLoadFloat4x4(&node.globalTransform_);

		if (node.skin_ > -1)
		{
			const Skin& skin = skins_.at(node.skin_);
			_ASSERT_EXPR(skin.joints_.size() <= Max_Joints, L"The size of the joint array is insufficient, please expand it.");
			for (size_t joint_index = 0; joint_index < skin.joints_.size(); ++joint_index)
			{
				XMStoreFloat4x4(&jointConstants_->data_.matrices_[joint_index],
					XMLoadFloat4x4(&skin.inverseBindMatrices_.at(joint_index)) *
					XMLoadFloat4x4(&animatedNodes.at(skin.joints_.at(joint_index)).globalTransform_) *
					XMMatrixInverse(NULL, globalTransform)
				);
			}
			jointConstants_->Activate(Joint_Slot, CBufferUsage::v);
		}
		if (node.mesh_ > -1)
		{
			const Mesh& mesh = meshes_.at(node.mesh_);
			for (std::vector<Primitive>::const_reference primitive : mesh.primitives_)
			{
				const Material& material = materials_.at(primitive.material_ < 0 ? materials_.size() - 1 : primitive.material_);
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
					primitive.vertexBufferViews_.at("JOINTS_0").buffer_ > -1 ? buffers_.at(primitive.vertexBufferViews_.at("JOINTS_0").buffer_).Get() : NULL,
					primitive.vertexBufferViews_.at("WEIGHTS_0").buffer_ > -1 ? buffers_.at(primitive.vertexBufferViews_.at("WEIGHTS_0").buffer_).Get() : NULL,
				};
				UINT strides[] =
				{
					static_cast<UINT>(primitive.vertexBufferViews_.at("POSITION").strideInBytes_),
					static_cast<UINT>(primitive.vertexBufferViews_.at("NORMAL").strideInBytes_),
					static_cast<UINT>(primitive.vertexBufferViews_.at("TANGENT").strideInBytes_),
					static_cast<UINT>(primitive.vertexBufferViews_.at("TEXCOORD_0").strideInBytes_),
					static_cast<UINT>(primitive.vertexBufferViews_.at("TEXCOORD_1").strideInBytes_),
					static_cast<UINT>(primitive.vertexBufferViews_.at("JOINTS_0").strideInBytes_),
					static_cast<UINT>(primitive.vertexBufferViews_.at("WEIGHTS_0").strideInBytes_),
				};
				UINT offsets[_countof(vertexBuffers)] = { 0 };
				deviceContext->IASetVertexBuffers(0, _countof(vertexBuffers), vertexBuffers, strides, offsets);

				// The slot numbers of 'primitive_textures' are defined in 'material.hlsli'.
				const int textureIndices[] =
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
				ID3D11ShaderResourceView* nullShaderResourceView = {};
				std::vector<ID3D11ShaderResourceView*> shaderResourceViews(_countof(textureIndices));
				for (int textureIndex = 0; textureIndex < shaderResourceViews.size(); ++textureIndex)
				{
					shaderResourceViews.at(textureIndex) = textureIndices[textureIndex] > -1 ? textureResourceViews_.at(textures_.at(textureIndices[textureIndex]).source_).Get() : nullShaderResourceView;
				}
				deviceContext->PSSetShaderResources(1, static_cast<UINT>(shaderResourceViews.size()), shaderResourceViews.data());

				XMStoreFloat4x4(&primitiveConstants_->data_.transform_, globalTransform * worldTransform);
				primitiveConstants_->data_.color_ = color;
				primitiveConstants_->data_.material_ = primitive.material_ < 0 ? static_cast<int>(materials_.size() - 1) : primitive.material_;
				primitiveConstants_->data_.skin_ = node.skin_;
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
		}
		for (decltype(node.children_)::value_type childIndex : node.children_)
		{
			traverse(childIndex);
		}
	};
	const Scene& scene = scenes_.at(sceneIndex);
	for (decltype(scene.nodes_)::value_type nodeIndex : scene.nodes_)
	{
		traverse(nodeIndex);
	}
	return drawcallCount;
}
int GltfSkeletalMesh::CastShadow(const DirectX::XMFLOAT4X4& world, const std::vector<Node>& animatedNodes, size_t sceneIndex)
{
	ID3D11DeviceContext* deviceContext = DXSystem::deviceContext_.Get();

	_ASSERT_EXPR(animatedNodes.size() == nodes_.size(), L"");

	int drawcalCount = 0;

	using namespace DirectX;
	XMMATRIX worldTransform = XMLoadFloat4x4(&world);

	for (int pass = 0; pass < 2; ++pass)
	{
		std::function<void(int)> traverse = [&](int nodeIndex) {
			const Node& node = animatedNodes.at(nodeIndex);
			XMMATRIX globalTransform = XMLoadFloat4x4(&node.globalTransform_);

			if (node.skin_ > -1)
			{
				const Skin& skin = skins_.at(node.skin_);
				_ASSERT_EXPR(skin.joints_.size() <= Max_Joints, L"The size of the joint array is insufficient, please expand it.");
				for (size_t joint_index = 0; joint_index < skin.joints_.size(); ++joint_index)
				{
					XMStoreFloat4x4(&jointConstants_->data_.matrices_[joint_index],
						XMLoadFloat4x4(&skin.inverseBindMatrices_.at(joint_index)) *
						XMLoadFloat4x4(&animatedNodes.at(skin.joints_.at(joint_index)).globalTransform_) *
						XMMatrixInverse(NULL, globalTransform)
					);
				}
				jointConstants_->Activate(Joint_Slot, CBufferUsage::v);
			}
			if (node.mesh_ > -1)
			{
				const Mesh& mesh = meshes_.at(node.mesh_);
				for (std::vector<Primitive>::const_reference primitive : mesh.primitives_)
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

					if (pass == 0)
					{
						deviceContext->VSSetShader(csmOpaqueSkeletalMeshVs_.Get(), NULL, 0);
						deviceContext->GSSetShader(csmOpaqueGs_.Get(), NULL, 0);
						deviceContext->PSSetShader(NULL, NULL, 0);
						deviceContext->IASetInputLayout(csmOpaqueInputLayout_.Get());

						ID3D11Buffer* vertex_buffers[] =
						{
							primitive.vertexBufferViews_.at("POSITION").buffer_ > -1 ? buffers_.at(primitive.vertexBufferViews_.at("POSITION").buffer_).Get() : NULL,
							primitive.vertexBufferViews_.at("JOINTS_0").buffer_ > -1 ? buffers_.at(primitive.vertexBufferViews_.at("JOINTS_0").buffer_).Get() : NULL,
							primitive.vertexBufferViews_.at("WEIGHTS_0").buffer_ > -1 ? buffers_.at(primitive.vertexBufferViews_.at("WEIGHTS_0").buffer_).Get() : NULL,
						};
						UINT strides[] =
						{
							static_cast<UINT>(primitive.vertexBufferViews_.at("POSITION").strideInBytes_),
							static_cast<UINT>(primitive.vertexBufferViews_.at("JOINTS_0").strideInBytes_),
							static_cast<UINT>(primitive.vertexBufferViews_.at("WEIGHTS_0").strideInBytes_),
						};
						UINT offsets[_countof(vertex_buffers)] = { 0 };
						deviceContext->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
					}
					else
					{
						deviceContext->VSSetShader(csmTransparentSkeletalMeshVs_.Get(), NULL, 0);
						deviceContext->GSSetShader(csmTransparentGs_.Get(), NULL, 0);
						deviceContext->PSSetShader(csmTransparentPs_.Get(), NULL, 0);
						deviceContext->IASetInputLayout(csm_transparent_input_layout.Get());

						ID3D11Buffer* vertex_buffers[4] =
						{
							primitive.vertexBufferViews_.at("POSITION").buffer_ > -1 ? buffers_.at(primitive.vertexBufferViews_.at("POSITION").buffer_).Get() : NULL,
							primitive.vertexBufferViews_.at("JOINTS_0").buffer_ > -1 ? buffers_.at(primitive.vertexBufferViews_.at("JOINTS_0").buffer_).Get() : NULL,
							primitive.vertexBufferViews_.at("WEIGHTS_0").buffer_ > -1 ? buffers_.at(primitive.vertexBufferViews_.at("WEIGHTS_0").buffer_).Get() : NULL,
							NULL,
						};
						UINT strides[4] =
						{
							static_cast<UINT>(primitive.vertexBufferViews_.at("POSITION").strideInBytes_),
							static_cast<UINT>(primitive.vertexBufferViews_.at("JOINTS_0").strideInBytes_),
							static_cast<UINT>(primitive.vertexBufferViews_.at("WEIGHTS_0").strideInBytes_),
							0,
						};
						int texcoord_ = material.data_.pbrMetallicRoughness_.basecolorTexture_.texcoord_;
						switch (texcoord_)
						{
						case 0:
							vertex_buffers[3] = buffers_.at(primitive.vertexBufferViews_.at("TEXCOORD_0").buffer_).Get();
							strides[3] = static_cast<UINT>(primitive.vertexBufferViews_.at("TEXCOORD_0").strideInBytes_);
							break;
						case 1:
							vertex_buffers[3] = buffers_.at(primitive.vertexBufferViews_.at("TEXCOORD_1").buffer_).Get();
							strides[3] = static_cast<UINT>(primitive.vertexBufferViews_.at("TEXCOORD_1").strideInBytes_);
							break;
						}

						UINT offsets[_countof(vertex_buffers)] = { 0 };
						deviceContext->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);

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

					XMStoreFloat4x4(&primitiveConstants_->data_.transform_, globalTransform * worldTransform);
					primitiveConstants_->data_.skin_ = node.skin_;
					primitiveConstants_->data_.startInstanceLocation_ = 0;
					primitiveConstants_->Activate(Primitive_Slot, CBufferUsage::v);

					deviceContext->IASetPrimitiveTopology(primitive.topology_);

					drawcalCount++;
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
			for (decltype(node.children_)::value_type childIndex : node.children_)
			{
				traverse(childIndex);
			}
		};
		const Scene& scene = scenes_.at(sceneIndex);
		for (decltype(scene.nodes_)::value_type nodeIndex : scene.nodes_)
		{
			traverse(nodeIndex);
		}
	}

	deviceContext->VSSetShader(NULL, NULL, 0);
	deviceContext->PSSetShader(NULL, NULL, 0);
	deviceContext->GSSetShader(NULL, NULL, 0);
	deviceContext->IASetInputLayout(NULL);

	return drawcalCount;
}