// StaticMeshBatching
#include "StaticMeshBatching.h"
#include <stack>
#include <functional>
#include <filesystem>

#include "Misc.h"
#include "Shader.h"
#include "Texture.h"

using namespace AbyssEngine;
using namespace std;

bool NullLoadImageData(tinygltf::Image*, const int, std::string*, std::string*, int, int, const unsigned char*, int, void*);

StaticMeshBatching::StaticMeshBatching(const std::string& filename) : filename_(filename)
{
	tinygltf::TinyGLTF tiny_gltf;
	tiny_gltf.SetImageLoader(NullLoadImageData, nullptr);

	tinygltf::Model gltfModel;
	std::string error, warning;
	bool succeeded{ false };
	if (filename.find(".glb") != std::string::npos)
	{
		succeeded = tiny_gltf.LoadBinaryFromFile(&gltfModel, &error, &warning, filename.c_str());
	}
	else if (filename.find(".gltf") != std::string::npos)
	{
		succeeded = tiny_gltf.LoadASCIIFromFile(&gltfModel, &error, &warning, filename.c_str());
	}

	_ASSERT_EXPR_A(warning.empty(), warning.c_str());
	_ASSERT_EXPR_A(error.empty(), error.c_str());
	_ASSERT_EXPR_A(succeeded, L"Failed to load glTF file");

	for (std::vector<tinygltf::Scene>::const_reference gltfScene : gltfModel.scenes)
	{
		Scene& scene{ scenes_.emplace_back() };
		scene.name_ = gltfModel.scenes.at(0).name;
		scene.nodes_ = gltfModel.scenes.at(0).nodes;
	}

	FetchNodes(gltfModel);
	FetchMeshes(gltfModel);
	FetchMaterials(gltfModel);
	FetchTextures(gltfModel);

	// TODO: This is a force-brute programming, may cause bugs.
	const std::map<std::string, BufferView>& vertexBufferViews = primitives_.at(0).vertexBufferViews_;
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{ "POSITION", 0, vertexBufferViews.at("POSITION").format_, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, vertexBufferViews.at("NORMAL").format_, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, vertexBufferViews.at("TANGENT").format_, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, vertexBufferViews.at("TEXCOORD_0").format_, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	vertexShader_ = Shader<ID3D11VertexShader>::Emplace("./Resources/Shader/GltfMeshStaticBatchingVS.cso",inputLayout_.ReleaseAndGetAddressOf(), input_element_desc, _countof(input_element_desc));
	pixelShader_ = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/GltfMeshPS.cso");

	D3D11_BUFFER_DESC bd{};
	bd.ByteWidth = sizeof(PrimitiveConstants);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HRESULT hr;
	hr = DXSystem::device_->CreateBuffer(&bd, nullptr, primitiveCBuffer_.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));


}
void StaticMeshBatching::FetchNodes(const tinygltf::Model& gltfModel)
{
	for (std::vector<tinygltf::Node>::const_reference gltfNode : gltfModel.nodes)
	{
		Node& node{ nodes_.emplace_back() };
		node.name_ = gltfNode.name;
		node.skin_ = gltfNode.skin;
		node.mesh_ = gltfNode.mesh;
		node.children_ = gltfNode.children;
		if (!gltfNode.matrix.empty())
		{
			Matrix matrix;
			for (size_t row = 0; row < 4; row++)
			{
				for (size_t column = 0; column < 4; column++)
				{
					matrix(row, column) = static_cast<float>(gltfNode.matrix.at(4 * row + column));
				}
			}

			DirectX::XMVECTOR S, T, R;
			bool succeed = DirectX::XMMatrixDecompose(&S, &R, &T, DirectX::XMLoadFloat4x4(&matrix));
			_ASSERT_EXPR(succeed, L"Failed to decompose matrix.");

			DirectX::XMStoreFloat3(&node.scale_, S);
			DirectX::XMStoreFloat4(&node.rotation_, R);
			DirectX::XMStoreFloat3(&node.translation_, T);
		}
		else
		{
			if (gltfNode.scale.size() > 0)
			{
				node.scale_.x = static_cast<float>(gltfNode.scale.at(0));
				node.scale_.y = static_cast<float>(gltfNode.scale.at(1));
				node.scale_.z = static_cast<float>(gltfNode.scale.at(2));
			}
			if (gltfNode.translation.size() > 0)
			{
				node.translation_.x = static_cast<float>(gltfNode.translation.at(0));
				node.translation_.y = static_cast<float>(gltfNode.translation.at(1));
				node.translation_.z = static_cast<float>(gltfNode.translation.at(2));
			}
			if (gltfNode.rotation.size() > 0)
			{
				node.rotation_.x = static_cast<float>(gltfNode.rotation.at(0));
				node.rotation_.y = static_cast<float>(gltfNode.rotation.at(1));
				node.rotation_.z = static_cast<float>(gltfNode.rotation.at(2));
				node.rotation_.w = static_cast<float>(gltfNode.rotation.at(3));
			}
		}
	}
	CumulateTransforms(nodes_);
}
void StaticMeshBatching::CumulateTransforms(std::vector<Node>& nodes)
{
	std::stack<Matrix> parentGlobalTransforms;
	std::function<void(int)> traverse{ [&](int node_index)->void
		{
			Node& node{ nodes.at(node_index) };
			Matrix S{ Matrix::CreateScale(node.scale_.x, node.scale_.y, node.scale_.z) };
			Matrix R{ Matrix::CreateFromQuaternion(Quaternion(node.rotation_.x, node.rotation_.y, node.rotation_.z, node.rotation_.w))};
			Matrix T{ Matrix::CreateTranslation(node.translation_.x, node.translation_.y, node.translation_.z) };
			node.globalTransform_ = S * R * T * parentGlobalTransforms.top();
			for (int child_index : node.children_)
			{
				parentGlobalTransforms.push(node.globalTransform_);
				traverse(child_index);
				parentGlobalTransforms.pop();
			}
	} };
	for (std::vector<int>::value_type node_index : scenes_.at(0).nodes_)
	{
		parentGlobalTransforms.push({ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });
		traverse(node_index);
		parentGlobalTransforms.pop();
	}
}
StaticMeshBatching::BufferView StaticMeshBatching::MakeBufferView(const tinygltf::Accessor& accessor)
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
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			bufferView.format_ = DXGI_FORMAT_R32G32_FLOAT;
			bufferView.strideInBytes_ = sizeof(FLOAT) * 2;
			break;
		default:
			_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
			break;
		}
		break;
	case TINYGLTF_TYPE_VEC3:
		switch (accessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			bufferView.format_ = DXGI_FORMAT_R32G32B32_FLOAT;
			bufferView.strideInBytes_ = sizeof(FLOAT) * 3;
			break;
		default:
			_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
			break;
		}
		break;
	case TINYGLTF_TYPE_VEC4:
		switch (accessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			bufferView.format_ = DXGI_FORMAT_R16G16B16A16_UINT;
			bufferView.strideInBytes_ = sizeof(USHORT) * 4;
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			bufferView.format_ = DXGI_FORMAT_R32G32B32A32_UINT;
			bufferView.strideInBytes_ = sizeof(UINT) * 4;
			break;
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			bufferView.format_ = DXGI_FORMAT_R32G32B32A32_FLOAT;
			bufferView.strideInBytes_ = sizeof(FLOAT) * 4;
			break;
		default:
			_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
			break;
		}
		break;
	default:
		_ASSERT_EXPR(FALSE, L"This accessor type is not supported.");
		break;
	}
	bufferView.sizeInBytes_ = static_cast<UINT>(accessor.count * bufferView.strideInBytes_);
	return bufferView;
}
void StaticMeshBatching::FetchMeshes(const tinygltf::Model& gltfModel)
{
	ID3D11Device* device = DXSystem::device_.Get();

	HRESULT hr = S_OK;

	struct combinedBuffer
	{
		size_t index_count;
		size_t vertex_count;

		D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		std::vector<unsigned int> indices;
		struct StructureOfArrays
		{
			std::vector<Vector3> positions;
			std::vector<Vector3> normals;
			std::vector<Vector4> tangents;
			std::vector<Vector2> texcoords;
		};
		StructureOfArrays vertices;
	};
	std::unordered_map<int/*material*/, combinedBuffer> combinedBuffers;

	// Collect primitives with same material
	//decltyoe:�w�肵��������^�������Ŏ擾���Ă���
	for (decltype(nodes_)::reference node : nodes_)
	{
		const Matrix transform = node.globalTransform_;

		if (node.mesh_ > -1)
		{
			const tinygltf::Mesh& gltfMesh = gltfModel.meshes.at(node.mesh_);

			for (std::vector<tinygltf::Primitive>::const_reference gltfPrimitive : gltfMesh.primitives)
			{
				combinedBuffer& combinedBuffer = combinedBuffers[gltfPrimitive.material];
				if (gltfPrimitive.indices > -1)
				{
					const tinygltf::Accessor gltfAccessor = gltfModel.accessors.at(gltfPrimitive.indices);
					const tinygltf::BufferView& gltfBufferView = gltfModel.bufferViews.at(gltfAccessor.bufferView);

					if (gltfAccessor.count == 0)
					{
						continue;
					}

					const size_t vertexOffset = combinedBuffer.vertices.positions.size();
					if (gltfAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
					{
						const unsigned char* buffer = gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset;
						for (size_t accessorIndex = 0; accessorIndex < gltfAccessor.count; ++accessorIndex)
						{
							combinedBuffer.indices.emplace_back(static_cast<unsigned int>(buffer[accessorIndex] + vertexOffset));
						}
					}
					else if (gltfAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
					{
						const unsigned short* buffer = reinterpret_cast<const unsigned short*>(gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset);
						for (size_t accessorIndex = 0; accessorIndex < gltfAccessor.count; ++accessorIndex)
						{
							combinedBuffer.indices.emplace_back(static_cast<unsigned int>(buffer[accessorIndex] + vertexOffset));
						}
					}
					else if (gltfAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
					{
						const unsigned int* buffer = reinterpret_cast<const unsigned int*>(gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset);
						for (size_t accessorIndex = 0; accessorIndex < gltfAccessor.count; ++accessorIndex)
						{
							combinedBuffer.indices.emplace_back(static_cast<unsigned int>(buffer[accessorIndex] + vertexOffset));
						}
					}
					else
					{
						_ASSERT_EXPR(false, L"This index format is not supported.");
					}
				}

				// Combine primitives using the same material into a single vertex buffer. In addition, apply a coordinate transformation to position, normal and tangent of primitives.
				for (decltype(gltfPrimitive.attributes)::const_reference gltf_attribute : gltfPrimitive.attributes)
				{
					const tinygltf::Accessor gltfAccessor = gltfModel.accessors.at(gltf_attribute.second);
					const tinygltf::BufferView& gltfBufferView = gltfModel.bufferViews.at(gltfAccessor.bufferView);

					if (gltfAccessor.count == 0)
					{
						continue;
					}

					if (gltf_attribute.first == "POSITION")
					{
						_ASSERT_EXPR(gltfAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && gltfAccessor.type == TINYGLTF_TYPE_VEC3, L"'POSITION' attribute must be of type TINYGLTF_COMPONENT_TYPE_FLOAT & TINYGLTF_TYPE_VEC3.");
						const Vector3* buffer = reinterpret_cast<const Vector3*>(gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset);
						for (size_t accessorIndex = 0; accessorIndex < gltfAccessor.count; ++accessorIndex)
						{
							Vector3 position = buffer[accessorIndex];
							DirectX::XMStoreFloat3(&position, XMVector3TransformCoord(XMLoadFloat3(&position), transform));
							combinedBuffer.vertices.positions.emplace_back(position);
						}
					}
					else if (gltf_attribute.first == "NORMAL")
					{
						_ASSERT_EXPR(gltfAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && gltfAccessor.type == TINYGLTF_TYPE_VEC3, L"'NORMAL' attribute must be of type TINYGLTF_COMPONENT_TYPE_FLOAT & TINYGLTF_TYPE_VEC3.");
						const Vector3* buffer = reinterpret_cast<const Vector3*>(gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset);
						for (size_t accessorIndex = 0; accessorIndex < gltfAccessor.count; ++accessorIndex)
						{
							Vector3 normal = buffer[accessorIndex];
							XMStoreFloat3(&normal, XMVector3TransformNormal(XMLoadFloat3(&normal), transform));
							combinedBuffer.vertices.normals.emplace_back(normal);
						}
					}
					else if (gltf_attribute.first == "TANGENT")
					{
						_ASSERT_EXPR(gltfAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && gltfAccessor.type == TINYGLTF_TYPE_VEC4, L"'TANGENT' attribute must be of type TINYGLTF_COMPONENT_TYPE_FLOAT & TINYGLTF_TYPE_VEC4.");
						const Vector4* buffer = reinterpret_cast<const Vector4*>(gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset);
						for (size_t accessorIndex = 0; accessorIndex < gltfAccessor.count; ++accessorIndex)
						{
							Vector4 tangent = buffer[accessorIndex];
							float sigma = tangent.w;
							tangent.w = 0;
							XMStoreFloat4(&tangent, XMVector4Transform(XMLoadFloat4(&tangent), transform));
							tangent.w = sigma;
							combinedBuffer.vertices.tangents.emplace_back(tangent);
						}
					}
					else if (gltf_attribute.first == "TEXCOORD_0")
					{
						_ASSERT_EXPR(gltfAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && gltfAccessor.type == TINYGLTF_TYPE_VEC2, L"'TEXCOORD_0' attribute must be of type TINYGLTF_COMPONENT_TYPE_FLOAT & TINYGLTF_TYPE_VEC2.");
						const Vector2* buffer = reinterpret_cast<const Vector2*>(gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset);
						for (size_t accessorIndex = 0; accessorIndex < gltfAccessor.count; ++accessorIndex)
						{
							combinedBuffer.vertices.texcoords.emplace_back(buffer[accessorIndex]);
						}
					}
				}
			}
		}
	}

	// Create GPU buffers
	for (decltype(combinedBuffers)::const_reference combinedBuffer : combinedBuffers)
	{
#if 1
		if (combinedBuffer.second.vertices.positions.size() == 0)
		{
			continue;
		}
#endif
		Primitive& primitive = primitives_.emplace_back();
		primitive.material_ = combinedBuffer.first;

		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		if (combinedBuffer.second.indices.size() > 0)
		{
			primitive.indexBufferView_.format_ = DXGI_FORMAT_R32_UINT;
			primitive.indexBufferView_.strideInBytes_ = sizeof(UINT);
			primitive.indexBufferView_.sizeInBytes_ = combinedBuffer.second.indices.size() * primitive.indexBufferView_.strideInBytes_;

			buffer_desc.ByteWidth = static_cast<UINT>(primitive.indexBufferView_.sizeInBytes_);
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			buffer_desc.CPUAccessFlags = 0;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = 0;
			subresource_data.pSysMem = combinedBuffer.second.indices.data();
			subresource_data.SysMemPitch = 0;
			subresource_data.SysMemSlicePitch = 0;
			hr = device->CreateBuffer(&buffer_desc, &subresource_data, primitive.indexBufferView_.buffer_.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		BufferView vertexBufferView;
		if (combinedBuffer.second.vertices.positions.size() > 0)
		{
			vertexBufferView.format_ = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexBufferView.strideInBytes_ = sizeof(FLOAT) * 3;
			vertexBufferView.sizeInBytes_ = combinedBuffer.second.vertices.positions.size() * vertexBufferView.strideInBytes_;

			buffer_desc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			subresource_data.pSysMem = combinedBuffer.second.vertices.positions.data();
			hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertexBufferView.buffer_.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
			primitive.vertexBufferViews_.emplace(std::make_pair("POSITION", vertexBufferView));
		}
		if (combinedBuffer.second.vertices.normals.size() > 0)
		{
			vertexBufferView.format_ = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexBufferView.strideInBytes_ = sizeof(FLOAT) * 3;
			vertexBufferView.sizeInBytes_ = combinedBuffer.second.vertices.normals.size() * vertexBufferView.strideInBytes_;

			buffer_desc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			subresource_data.pSysMem = combinedBuffer.second.vertices.normals.data();
			hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertexBufferView.buffer_.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
			primitive.vertexBufferViews_.emplace(std::make_pair("NORMAL", vertexBufferView));
		}
		if (combinedBuffer.second.vertices.tangents.size() > 0)
		{
			vertexBufferView.format_ = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexBufferView.strideInBytes_ = sizeof(FLOAT) * 4;
			vertexBufferView.sizeInBytes_ = combinedBuffer.second.vertices.tangents.size() * vertexBufferView.strideInBytes_;

			buffer_desc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			subresource_data.pSysMem = combinedBuffer.second.vertices.tangents.data();
			hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertexBufferView.buffer_.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
			primitive.vertexBufferViews_.emplace(std::make_pair("TANGENT", vertexBufferView));
		}
		if (combinedBuffer.second.vertices.texcoords.size() > 0)
		{
			vertexBufferView.format_ = DXGI_FORMAT_R32G32_FLOAT;
			vertexBufferView.strideInBytes_ = sizeof(FLOAT) * 2;
			vertexBufferView.sizeInBytes_ = combinedBuffer.second.vertices.texcoords.size() * vertexBufferView.strideInBytes_;

			buffer_desc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			subresource_data.pSysMem = combinedBuffer.second.vertices.texcoords.data();
			hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertexBufferView.buffer_.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
			primitive.vertexBufferViews_.emplace(std::make_pair("TEXCOORD_0", vertexBufferView));
		}


		// Add dummy attributes if any are missing.
		const std::unordered_map<std::string, BufferView> attributes =
		{
			{ "POSITION", { DXGI_FORMAT_R32G32B32_FLOAT } },
			{ "NORMAL", { DXGI_FORMAT_R32G32B32_FLOAT } },
			{ "TANGENT", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
			{ "TEXCOORD_0", { DXGI_FORMAT_R32G32_FLOAT } },
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

void StaticMeshBatching::Render(const Matrix& world)
{
	ID3D11DeviceContext* deviceContext = DXSystem::deviceContext_.Get();
	deviceContext->PSSetShaderResources(0, 1, materialResourceView_.GetAddressOf());

	deviceContext->VSSetShader(vertexShader_.Get(), nullptr, 0);
	deviceContext->PSSetShader(pixelShader_.Get(), nullptr, 0);
	deviceContext->IASetInputLayout(inputLayout_.Get());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (decltype(primitives_)::const_reference primitive : primitives_)
	{
		ID3D11Buffer* vertexBuffer[] = {
			primitive.vertexBufferViews_.at("POSITION").buffer_.Get(),
			primitive.vertexBufferViews_.at("NORMAL").buffer_.Get(),
			primitive.vertexBufferViews_.at("TANGENT").buffer_.Get(),
			primitive.vertexBufferViews_.at("TEXCOORD_0").buffer_.Get(),
		};
		UINT strides[] = {
			static_cast<UINT>(primitive.vertexBufferViews_.at("POSITION").strideInBytes_),
			static_cast<UINT>(primitive.vertexBufferViews_.at("NORMAL").strideInBytes_),
			static_cast<UINT>(primitive.vertexBufferViews_.at("TANGENT").strideInBytes_),
			static_cast<UINT>(primitive.vertexBufferViews_.at("TEXCOORD_0").strideInBytes_),
		};
		UINT offsets[_countof(vertexBuffer)] = {};
		deviceContext->IASetVertexBuffers(0, _countof(vertexBuffer), vertexBuffer, strides, offsets);
		deviceContext->IASetIndexBuffer(primitive.indexBufferView_.buffer_.Get(), primitive.indexBufferView_.format_, 0);

		PrimitiveConstants primitiveData_ = {};
		primitiveData_.material_ = primitive.material_;
		primitiveData_.hasTangent_ = primitive.vertexBufferViews_.at("TANGENT").buffer_ != NULL;
		primitiveData_.world_ = world;
		deviceContext->UpdateSubresource(primitiveCBuffer_.Get(), 0, 0, &primitiveData_, 0, 0);
		deviceContext->VSSetConstantBuffers(1, 1, primitiveCBuffer_.GetAddressOf());
		deviceContext->PSSetConstantBuffers(1, 1, primitiveCBuffer_.GetAddressOf());

		const Material& material = materials_.at(primitive.material_);
		const int texture_indices[] =
		{
			material.data_.pbrMetallicRoughness_.basecolorTexture_.index_,
			material.data_.pbrMetallicRoughness_.metallicRoughnessTexture_.index_,
			material.data_.normalTexture_.index_,
			material.data_.emissiveTexture_.index_,
			material.data_.occlusionTexture_.index_,
		};
		ID3D11ShaderResourceView* null_shader_resource_view = {};
		std::vector<ID3D11ShaderResourceView*> shaderResourceViews_(_countof(texture_indices));
		for (int textureIndex_ = 0; textureIndex_ < shaderResourceViews_.size(); ++textureIndex_)
		{
			shaderResourceViews_.at(textureIndex_) = texture_indices[textureIndex_] > -1 ? textureResourceViews_.at(textures_.at(texture_indices[textureIndex_]).source_).Get() : null_shader_resource_view;
		}
		deviceContext->PSSetShaderResources(1, static_cast<UINT>(shaderResourceViews_.size()), shaderResourceViews_.data());

		deviceContext->DrawIndexed(static_cast<UINT>(primitive.indexBufferView_.Count()), 0, 0);

	}
}

void StaticMeshBatching::FetchMaterials(const tinygltf::Model& gltfModel)
{
	ID3D11Device* device = DXSystem::device_.Get();

	for (std::vector<tinygltf::Material>::const_reference gltf_material : gltfModel.materials)
	{
		std::vector<Material>::reference material = materials_.emplace_back();

		material.name_ = gltf_material.name;

		material.data_.emissiveFactor_[0] = static_cast<float>(gltf_material.emissiveFactor.at(0));
		material.data_.emissiveFactor_[1] = static_cast<float>(gltf_material.emissiveFactor.at(1));
		material.data_.emissiveFactor_[2] = static_cast<float>(gltf_material.emissiveFactor.at(2));

		material.data_.alphaMode_ = gltf_material.alphaMode == "OPAQUE" ? 0 : gltf_material.alphaMode == "MASK" ? 1 : gltf_material.alphaMode == "BLEND" ? 2 : 0;
		material.data_.alphaCutoff_ = static_cast<float>(gltf_material.alphaCutoff);
		material.data_.doubleSided_ = gltf_material.doubleSided ? 1 : 0;

		material.data_.pbrMetallicRoughness_.basecolorFactor_[0] = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(0));
		material.data_.pbrMetallicRoughness_.basecolorFactor_[1] = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(1));
		material.data_.pbrMetallicRoughness_.basecolorFactor_[2] = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(2));
		material.data_.pbrMetallicRoughness_.basecolorFactor_[3] = static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(3));
		material.data_.pbrMetallicRoughness_.basecolorTexture_.index_ = gltf_material.pbrMetallicRoughness.baseColorTexture.index;
		material.data_.pbrMetallicRoughness_.basecolorTexture_.texcoord_ = gltf_material.pbrMetallicRoughness.baseColorTexture.texCoord;
		material.data_.pbrMetallicRoughness_.metallicFactor_ = static_cast<float>(gltf_material.pbrMetallicRoughness.metallicFactor);
		material.data_.pbrMetallicRoughness_.roughnessFactor_ = static_cast<float>(gltf_material.pbrMetallicRoughness.roughnessFactor);
		material.data_.pbrMetallicRoughness_.metallicRoughnessTexture_.index_ = gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index;
		material.data_.pbrMetallicRoughness_.metallicRoughnessTexture_.texcoord_ = gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.texCoord;

		material.data_.normalTexture_.index_ = gltf_material.normalTexture.index;
		material.data_.normalTexture_.texcoord_ = gltf_material.normalTexture.texCoord;
		material.data_.normalTexture_.scale_ = static_cast<float>(gltf_material.normalTexture.scale);

		material.data_.occlusionTexture_.index_ = gltf_material.occlusionTexture.index;
		material.data_.occlusionTexture_.texcoord_ = gltf_material.occlusionTexture.texCoord;
		material.data_.occlusionTexture_.strength_ = static_cast<float>(gltf_material.occlusionTexture.strength);

		material.data_.emissiveTexture_.index_ = gltf_material.emissiveTexture.index;
		material.data_.emissiveTexture_.texcoord_ = gltf_material.emissiveTexture.texCoord;
	}

	// Create material data as shader resource view on GPU
	std::vector<Material::CBuffer> material_data;
	for (std::vector<Material>::const_reference material : materials_)
	{
		material_data.emplace_back(material.data_);
	}

	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> material_buffer;
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(Material::CBuffer) * material_data.size());
	buffer_desc.StructureByteStride = sizeof(Material::CBuffer);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = material_data.data();
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, material_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
	shader_resource_view_desc.Format = DXGI_FORMAT_UNKNOWN;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	shader_resource_view_desc.Buffer.NumElements = static_cast<UINT>(material_data.size());
	hr = device->CreateShaderResourceView(material_buffer.Get(), &shader_resource_view_desc, materialResourceView_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}
void StaticMeshBatching::FetchTextures(const tinygltf::Model& gltfModel)
{
	ID3D11Device* device = DXSystem::device_.Get();

	HRESULT hr{ S_OK };
	for (const tinygltf::Texture& gltfTexture : gltfModel.textures)
	{
		Texture& texture{ textures_.emplace_back() };
		texture.name_ = gltfTexture.name;
		texture.source_ = gltfTexture.source;
	}
	for (const tinygltf::Image& gltfImage : gltfModel.images)
	{
		Image& image{ images_.emplace_back() };
		image.name_ = gltfImage.name;
		image.width_ = gltfImage.width;
		image.height_ = gltfImage.height;
		image.component_ = gltfImage.component;
		image.bits_ = gltfImage.bits;
		image.pixelType_ = gltfImage.pixel_type;
		image.bufferView_ = gltfImage.bufferView;
		image.mimeType_ = gltfImage.mimeType;
		image.uri_ = gltfImage.uri;
		image.asIs_ = gltfImage.as_is;

		if (gltfImage.bufferView > -1)
		{
			const tinygltf::BufferView& bufferView{ gltfModel.bufferViews.at(gltfImage.bufferView) };
			const tinygltf::Buffer& buffer{ gltfModel.buffers.at(bufferView.buffer) };
			const BYTE* data = buffer.data.data() + bufferView.byteOffset;

			ID3D11ShaderResourceView* textureResourceView_{};
			hr = AbyssEngine::Texture::LoadTextureFromMemory(data, bufferView.byteLength, &textureResourceView_);
			if (hr == S_OK)
			{
				textureResourceViews_.emplace_back().Attach(textureResourceView_);
			}
		}
		else
		{
			const std::filesystem::path path(filename_);
			ID3D11ShaderResourceView* shader_resource_view{};
			D3D11_TEXTURE2D_DESC texture2d_desc;
			std::string filename = path.parent_path().concat("/").string() + std::string(gltfImage.uri.begin(), gltfImage.uri.end());
			hr = AbyssEngine::Texture::LoadTextureFromFile(filename, & shader_resource_view, & texture2d_desc);
			if (hr == S_OK)
			{
				textureResourceViews_.emplace_back().Attach(shader_resource_view);
			}
		}
	}
}
