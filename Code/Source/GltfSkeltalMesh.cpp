//#include "GltfSkeltalMesh.h"
//
//#include <sstream>
//
//#include "Misc.h"
//#include "shader.h"
//#include "texture.h"
//#include "collision.h"
//
//using namespace DirectX;
//
//skeletal_mesh::skeletal_mesh(ID3D11Device* device, const std::string& filename) : geometric_substance(device, filename)
//{
//	HRESULT hr = S_OK;
//
//	tinygltf::TinyGLTF tiny_gltf;
//	tiny_gltf.SetImageLoader(null_load_image_data, nullptr);
//
//	tinygltf::Model transmission_model;
//	std::string error, warning;
//	bool succeeded{ false };
//	if (filename.find(".glb") != std::string::npos)
//	{
//		succeeded = tiny_gltf.LoadBinaryFromFile(&transmission_model, &error, &warning, filename.c_str());
//	}
//	else if (filename.find(".gltf") != std::string::npos)
//	{
//		succeeded = tiny_gltf.LoadASCIIFromFile(&transmission_model, &error, &warning, filename.c_str());
//	}
//	if (!warning.empty())
//	{
//		OutputDebugStringA(warning.c_str());
//	}
//	if (!error.empty())
//	{
//		throw std::exception(error.c_str());
//	}
//	if (!succeeded)
//	{
//		throw std::exception("Failed to load glTF file");
//	}
//
//	extract_assets(transmission_model);
//	extract_extensions(transmission_model);
//
//	extract_scenes(transmission_model);
//	extract_nodes(transmission_model);
//
//	extract_materials(device, transmission_model);
//	extract_textures(device, transmission_model);
//	extract_meshes(device, transmission_model);
//	extract_animations(transmission_model);
//
//	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
//	{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		{ "JOINTS", 0, DXGI_FORMAT_R16G16B16A16_UINT, 5, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "WEIGHTS", 0,DXGI_FORMAT_R32G32B32A32_FLOAT, 6, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//	create_vs_from_cso(device, "skeletal_mesh_vs.cso", skeletal_mesh_vs.ReleaseAndGetAddressOf(), input_layout.ReleaseAndGetAddressOf(), input_element_desc, _countof(input_element_desc));
//	create_ps_from_cso(device, "geometric_substance_ps.cso", skeletal_mesh_ps.ReleaseAndGetAddressOf());
//
//	D3D11_INPUT_ELEMENT_DESC csm_opaque_input_element_desc[] =
//	{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		{ "JOINTS", 0, DXGI_FORMAT_R16G16B16A16_UINT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "WEIGHTS", 0,DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//	create_vs_from_cso(device, "csm_opaque_skeletal_mesh_vs.cso", csm_opaque_skeletal_mesh_vs.ReleaseAndGetAddressOf(), csm_opaque_input_layout.ReleaseAndGetAddressOf(), csm_opaque_input_element_desc, _countof(csm_opaque_input_element_desc));
//	create_gs_from_cso(device, "csm_opaque_gs.cso", csm_opaque_gs.ReleaseAndGetAddressOf());
//
//	D3D11_INPUT_ELEMENT_DESC csm_transparent_input_element_desc[]
//	{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		{ "JOINTS", 0, DXGI_FORMAT_R16G16B16A16_UINT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "WEIGHTS", 0,DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//	create_vs_from_cso(device, "csm_transparent_skeletal_mesh_vs.cso", csm_transparent_skeletal_mesh_vs.ReleaseAndGetAddressOf(), csm_transparent_input_layout.ReleaseAndGetAddressOf(), csm_transparent_input_element_desc, _countof(csm_transparent_input_element_desc));
//	create_gs_from_cso(device, "csm_transparent_gs.cso", csm_transparent_gs.ReleaseAndGetAddressOf());
//	create_ps_from_cso(device, "csm_transparent_ps.cso", csm_transparent_ps.ReleaseAndGetAddressOf());
//
//	joint_constants = std::make_unique<decltype(joint_constants)::element_type>(device);
//}
//void skeletal_mesh::extract_meshes(ID3D11Device* device, const tinygltf::Model& transmission_model)
//{
//	HRESULT hr{ S_OK };
//
//	std::function<buffer_view(const tinygltf::Accessor&)> make_buffer_view = [](const tinygltf::Accessor& accessor)->buffer_view
//	{
//		buffer_view buffer_view;
//		switch (accessor.type)
//		{
//		case TINYGLTF_TYPE_SCALAR:
//			switch (accessor.componentType)
//			{
//			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
//				buffer_view.format = DXGI_FORMAT_R16_UINT;
//				buffer_view.stride_in_bytes = sizeof(USHORT);
//				break;
//			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
//				buffer_view.format = DXGI_FORMAT_R32_UINT;
//				buffer_view.stride_in_bytes = sizeof(UINT);
//				break;
//			default:
//				_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
//				break;
//			}
//			break;
//		case TINYGLTF_TYPE_VEC2:
//			switch (accessor.componentType)
//			{
//			case TINYGLTF_COMPONENT_TYPE_BYTE:
//				buffer_view.format = DXGI_FORMAT_R8G8_SINT;
//				buffer_view.stride_in_bytes = sizeof(CHAR) * 2;
//				break;
//			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
//				buffer_view.format = DXGI_FORMAT_R8G8_UINT;
//				buffer_view.stride_in_bytes = sizeof(BYTE) * 2;
//				break;
//			case TINYGLTF_COMPONENT_TYPE_SHORT:
//				buffer_view.format = DXGI_FORMAT_R16G16_SINT;
//				buffer_view.stride_in_bytes = sizeof(SHORT) * 2;
//				break;
//			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
//				buffer_view.format = DXGI_FORMAT_R16G16_UINT;
//				buffer_view.stride_in_bytes = sizeof(USHORT) * 2;
//				break;
//			case TINYGLTF_COMPONENT_TYPE_INT:
//				buffer_view.format = DXGI_FORMAT_R32G32_SINT;
//				buffer_view.stride_in_bytes = sizeof(INT) * 2;
//				break;
//			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
//				buffer_view.format = DXGI_FORMAT_R32G32_UINT;
//				buffer_view.stride_in_bytes = sizeof(UINT) * 2;
//				break;
//			case TINYGLTF_COMPONENT_TYPE_FLOAT:
//				buffer_view.format = DXGI_FORMAT_R32G32_FLOAT;
//				buffer_view.stride_in_bytes = sizeof(FLOAT) * 2;
//				break;
//			case TINYGLTF_COMPONENT_TYPE_DOUBLE:
//			default:
//				_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
//				break;
//			}
//			break;
//		case TINYGLTF_TYPE_VEC3:
//			switch (accessor.componentType)
//			{
//			case TINYGLTF_COMPONENT_TYPE_INT:
//				buffer_view.format = DXGI_FORMAT_R32G32B32_SINT;
//				buffer_view.stride_in_bytes = sizeof(INT) * 3;
//				break;
//			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
//				buffer_view.format = DXGI_FORMAT_R32G32B32_UINT;
//				buffer_view.stride_in_bytes = sizeof(UINT) * 3;
//				break;
//			case TINYGLTF_COMPONENT_TYPE_FLOAT:
//				buffer_view.format = DXGI_FORMAT_R32G32B32_FLOAT;
//				buffer_view.stride_in_bytes = sizeof(FLOAT) * 3;
//				break;
//			case TINYGLTF_COMPONENT_TYPE_BYTE:
//			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
//			case TINYGLTF_COMPONENT_TYPE_SHORT:
//			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
//			case TINYGLTF_COMPONENT_TYPE_DOUBLE:
//			default:
//				_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
//				break;
//			}
//			break;
//		case TINYGLTF_TYPE_VEC4:
//			switch (accessor.componentType)
//			{
//			case TINYGLTF_COMPONENT_TYPE_BYTE:
//				buffer_view.format = DXGI_FORMAT_R8G8B8A8_SINT;
//				buffer_view.stride_in_bytes = sizeof(CHAR) * 4;
//				break;
//			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
//				buffer_view.format = DXGI_FORMAT_R8G8B8A8_UINT;
//				buffer_view.stride_in_bytes = sizeof(BYTE) * 4;
//				break;
//			case TINYGLTF_COMPONENT_TYPE_SHORT:
//				buffer_view.format = DXGI_FORMAT_R16G16B16A16_SINT;
//				buffer_view.stride_in_bytes = sizeof(SHORT) * 4;
//				break;
//			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
//				buffer_view.format = DXGI_FORMAT_R16G16B16A16_UINT;
//				buffer_view.stride_in_bytes = sizeof(USHORT) * 4;
//				break;
//			case TINYGLTF_COMPONENT_TYPE_INT:
//				buffer_view.format = DXGI_FORMAT_R32G32B32A32_SINT;
//				buffer_view.stride_in_bytes = sizeof(INT) * 4;
//				break;
//			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
//				buffer_view.format = DXGI_FORMAT_R32G32B32A32_UINT;
//				buffer_view.stride_in_bytes = sizeof(UINT) * 4;
//				break;
//			case TINYGLTF_COMPONENT_TYPE_FLOAT:
//				buffer_view.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//				buffer_view.stride_in_bytes = sizeof(FLOAT) * 4;
//				break;
//			case TINYGLTF_COMPONENT_TYPE_DOUBLE:
//			default:
//				_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
//				break;
//			}
//			break;
//		default:
//			_ASSERT_EXPR(FALSE, L"This accessor type is not supported.");
//			break;
//		}
//		buffer_view.size_in_bytes = accessor.count * buffer_view.stride_in_bytes;
//		return buffer_view;
//	};
//
//	for (std::vector<tinygltf::Mesh>::const_reference transmission_mesh : transmission_model.meshes)
//	{
//		mesh& mesh = meshes.emplace_back();
//		mesh.name = transmission_mesh.name;
//
//		for (std::vector<tinygltf::Primitive>::const_reference transmission_primitive : transmission_mesh.primitives)
//		{
//			primitive& primitive = mesh.primitives.emplace_back();
//			primitive.material = transmission_primitive.material;
//			const std::unordered_map<size_t, D3D_PRIMITIVE_TOPOLOGY> topologies =
//			{
//				{ TINYGLTF_MODE_TRIANGLES, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST },
//				{ TINYGLTF_MODE_TRIANGLE_STRIP, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP },
//				{ TINYGLTF_MODE_POINTS, D3D_PRIMITIVE_TOPOLOGY_POINTLIST },
//				{ TINYGLTF_MODE_LINE, D3D_PRIMITIVE_TOPOLOGY_LINELIST },
//				{ TINYGLTF_MODE_LINE_STRIP, D3D_PRIMITIVE_TOPOLOGY_LINESTRIP },
//			};
//			primitive.topology = topologies.at(transmission_primitive.mode);
//
//			// Create index buffer
//			if (transmission_primitive.indices > -1)
//			{
//				tinygltf::Accessor transmission_accessor = transmission_model.accessors.at(transmission_primitive.indices);
//				const tinygltf::BufferView& transmission_buffer_view = transmission_model.bufferViews.at(transmission_accessor.bufferView);
//
//				const unsigned char* buffer = transmission_model.buffers.at(transmission_buffer_view.buffer).data.data() + transmission_buffer_view.byteOffset + transmission_accessor.byteOffset;
//				std::vector<unsigned short> extra_indices; // Use for TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE formatted index buffer
//				if (transmission_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
//				{
//					// Convert to TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT
//					extra_indices.resize(transmission_accessor.count);
//					for (size_t i = 0; i < transmission_accessor.count; ++i)
//					{
//						extra_indices.at(i) = static_cast<unsigned short>(buffer[i]);
//					}
//					transmission_accessor.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT;
//					buffer = reinterpret_cast<const unsigned char*>(extra_indices.data());
//				}
//				primitive.index_buffer_view = make_buffer_view(transmission_accessor);
//				primitive.index_buffer_view.buffer = static_cast<int>(buffers.size());
//				primitive.index_location = 0;
//				primitive.index_count = transmission_accessor.count;
//
//				D3D11_BUFFER_DESC buffer_desc = {};
//				D3D11_SUBRESOURCE_DATA subresource_data = {};
//				buffer_desc.ByteWidth = static_cast<UINT>(primitive.index_buffer_view.size_in_bytes);
//				buffer_desc.Usage = D3D11_USAGE_DEFAULT;
//				buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//				buffer_desc.CPUAccessFlags = 0;
//				buffer_desc.MiscFlags = 0;
//				buffer_desc.StructureByteStride = 0;
//				subresource_data.pSysMem = buffer;
//				subresource_data.SysMemPitch = 0;
//				subresource_data.SysMemSlicePitch = 0;
//				hr = device->CreateBuffer(&buffer_desc, &subresource_data, buffers.emplace_back().ReleaseAndGetAddressOf());
//				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//			}
//			// Create vertex buffers
//			for (std::map<std::string, int>::const_reference transmission_attribute : transmission_primitive.attributes)
//			{
//				tinygltf::Accessor transmission_accessor = transmission_model.accessors.at(transmission_attribute.second);
//				const tinygltf::BufferView& transmission_buffer_view = transmission_model.bufferViews.at(transmission_accessor.bufferView);
//
//				bool supported_attribute = true;
//
//				const unsigned char* buffer = transmission_model.buffers.at(transmission_buffer_view.buffer).data.data() + transmission_buffer_view.byteOffset + transmission_accessor.byteOffset;
//
//				std::vector<unsigned short> extra_joints;
//				std::vector<float> extra_weights;
//
//				// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#buffers-and-buffer-views
//				if (transmission_attribute.first == "POSITION")
//				{
//					// Constructing a bounding box
//					if (transmission_accessor.maxValues.size() == 3 && transmission_accessor.minValues.size() == 3)
//					{
//						primitive.max_value = { static_cast<float>(transmission_accessor.maxValues.at(0)), static_cast<float>(transmission_accessor.maxValues.at(1)), static_cast<float>(transmission_accessor.maxValues.at(2)) };
//						primitive.min_value = { static_cast<float>(transmission_accessor.minValues.at(0)), static_cast<float>(transmission_accessor.minValues.at(1)), static_cast<float>(transmission_accessor.minValues.at(2)) };
//					}
//				}
//				else if (transmission_attribute.first == "NORMAL")
//				{
//				}
//				else if (transmission_attribute.first == "TANGENT")
//				{
//				}
//				else if (transmission_attribute.first == "TEXCOORD_0")
//				{
//					_ASSERT_EXPR(transmission_accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && transmission_accessor.type == TINYGLTF_TYPE_VEC2, L"'TEXCOORD_0' attribute must be of type TINYGLTF_COMPONENT_TYPE_FLOAT & TINYGLTF_TYPE_VEC2.");
//				}
//				else if (transmission_attribute.first == "TEXCOORD_1")
//				{
//					_ASSERT_EXPR(transmission_accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && transmission_accessor.type == TINYGLTF_TYPE_VEC2, L"'TEXCOORD_0' attribute must be of type TINYGLTF_COMPONENT_TYPE_FLOAT & TINYGLTF_TYPE_VEC2.");
//				}
//				else if (transmission_attribute.first == "JOINTS_0")
//				{
//					_ASSERT_EXPR(transmission_accessor.type == TINYGLTF_TYPE_VEC4, L"'JOINTS_0' attribute must be of TINYGLTF_TYPE_VEC4.");
//					if (transmission_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT || transmission_accessor.componentType == TINYGLTF_COMPONENT_TYPE_SHORT)
//					{
//						transmission_accessor.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT;
//					}
//					else if (transmission_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE || transmission_accessor.componentType == TINYGLTF_COMPONENT_TYPE_BYTE)
//					{
//						// Convert to TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT
//						extra_joints.resize(transmission_accessor.count * 4);
//						for (size_t i = 0; i < transmission_accessor.count * 4; ++i)
//						{
//							extra_joints.at(i) = static_cast<unsigned short>(buffer[i]);
//						}
//						transmission_accessor.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT;
//						buffer = reinterpret_cast<unsigned char*>(extra_joints.data());
//					}
//					else
//					{
//						_ASSERT_EXPR(FALSE, L"This TINYGLTF_COMPONENT_TYPE of JOINTS_0 is not supported");
//					}
//				}
//				else if (transmission_attribute.first == "WEIGHTS_0")
//				{
//					_ASSERT_EXPR(transmission_accessor.type == TINYGLTF_TYPE_VEC4, L"'JOINTS_0' attribute must be of TINYGLTF_TYPE_VEC4.");
//					if (transmission_accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
//					{
//					}
//					else if (transmission_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
//					{
//						// Convert to TINYGLTF_COMPONENT_TYPE_FLOAT
//						extra_weights.resize(transmission_accessor.count * 4);
//						for (size_t accessor_index = 0; accessor_index < transmission_accessor.count; ++accessor_index)
//						{
//							extra_weights.at(accessor_index * 4 + 0) = static_cast<float>(buffer[accessor_index * 4 + 0]) / 0xFF;
//							extra_weights.at(accessor_index * 4 + 1) = static_cast<float>(buffer[accessor_index * 4 + 1]) / 0xFF;
//							extra_weights.at(accessor_index * 4 + 2) = static_cast<float>(buffer[accessor_index * 4 + 2]) / 0xFF;
//							extra_weights.at(accessor_index * 4 + 3) = static_cast<float>(buffer[accessor_index * 4 + 3]) / 0xFF;
//						}
//						transmission_accessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
//						buffer = reinterpret_cast<unsigned char*>(extra_weights.data());
//					}
//					else if (transmission_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
//					{
//						// Convert to TINYGLTF_COMPONENT_TYPE_FLOAT
//						extra_weights.resize(transmission_accessor.count * 4);
//						for (size_t accessor_index = 0; accessor_index < transmission_accessor.count; ++accessor_index)
//						{
//							extra_weights.at(accessor_index * 4 + 0) = static_cast<float>(reinterpret_cast<const unsigned short*>(buffer)[accessor_index * 4 + 0]) / 0xFFFF;
//							extra_weights.at(accessor_index * 4 + 1) = static_cast<float>(reinterpret_cast<const unsigned short*>(buffer)[accessor_index * 4 + 1]) / 0xFFFF;
//							extra_weights.at(accessor_index * 4 + 2) = static_cast<float>(reinterpret_cast<const unsigned short*>(buffer)[accessor_index * 4 + 2]) / 0xFFFF;
//							extra_weights.at(accessor_index * 4 + 3) = static_cast<float>(reinterpret_cast<const unsigned short*>(buffer)[accessor_index * 4 + 3]) / 0xFFFF;
//						}
//						transmission_accessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
//						buffer = reinterpret_cast<unsigned char*>(extra_weights.data());
//					}
//					else
//					{
//						_ASSERT_EXPR(FALSE, L"This TINYGLTF_COMPONENT_TYPE of WEIGHTS_0 is not supported");
//					}
//				}
//				else if (transmission_attribute.first == "COLOR_0")
//				{
//					supported_attribute = false;
//					OutputDebugStringA(std::string("We do not support vertex attribute : " + transmission_attribute.first + '\n').c_str());
//				}
//				else
//				{
//					supported_attribute = false;
//					OutputDebugStringA(std::string("We do not support vertex attribute : " + transmission_attribute.first + '\n').c_str());
//				}
//
//				if (supported_attribute)
//				{
//					buffer_view vertex_buffer_view = make_buffer_view(transmission_accessor);
//					vertex_buffer_view.buffer = static_cast<int>(buffers.size());
//
//					D3D11_BUFFER_DESC buffer_desc{};
//					D3D11_SUBRESOURCE_DATA subresource_data{};
//					buffer_desc.ByteWidth = static_cast<UINT>(vertex_buffer_view.size_in_bytes);
//					buffer_desc.Usage = D3D11_USAGE_DEFAULT;
//					buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//					buffer_desc.CPUAccessFlags = 0;
//					buffer_desc.MiscFlags = 0;
//					buffer_desc.StructureByteStride = 0;
//					subresource_data.pSysMem = buffer;
//					subresource_data.SysMemPitch = 0;
//					subresource_data.SysMemSlicePitch = 0;
//					hr = device->CreateBuffer(&buffer_desc, &subresource_data, buffers.emplace_back().ReleaseAndGetAddressOf());
//					_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//					primitive.vertex_buffer_views.emplace(std::make_pair(transmission_attribute.first, vertex_buffer_view));
//				}
//			}
//
//#if 1
//			std::function<std::tuple<const void*, size_t>(const std::string& attribute)> addressof = [&](const std::string& attribute)->std::tuple<const void*, size_t>
//			{
//				_ASSERT_EXPR(transmission_primitive.attributes.find(attribute) != transmission_primitive.attributes.end(), L"");
//
//				std::tuple<const void*, size_t> tuple_value;
//
//				std::vector<tinygltf::Accessor>::const_reference transmission_accessor = transmission_model.accessors.at(transmission_primitive.attributes.at(attribute));
//				std::vector<tinygltf::BufferView>::const_reference transmission_buffer_view = transmission_model.bufferViews.at(transmission_accessor.bufferView);
//
//				std::get<0>(tuple_value) = transmission_model.buffers.at(transmission_buffer_view.buffer).data.data() + transmission_buffer_view.byteOffset + transmission_accessor.byteOffset;
//				std::get<1>(tuple_value) = transmission_accessor.count;
//
//				return tuple_value;
//			};
//			// If this primitive does not have normals, calculate them.
//			std::vector<DirectX::XMFLOAT3> extra_normals;
//			if (primitive.vertex_buffer_views.find("NORMAL") == primitive.vertex_buffer_views.end())
//			{
//				std::tuple<const void*, size_t> tuple_value = addressof("POSITION");
//				const DirectX::XMFLOAT3* positions = reinterpret_cast<const DirectX::XMFLOAT3*>(std::get<0>(tuple_value));
//				const size_t vertex_count = std::get<1>(tuple_value);
//
//				extra_normals.resize(vertex_count);
//				DirectX::XMFLOAT3* normals = extra_normals.data();
//
//				_ASSERT_EXPR(transmission_primitive.indices > -1, L"An index buffer is needed for this process.");
//				std::vector<tinygltf::Accessor>::const_reference transmission_accessor = transmission_model.accessors.at(transmission_primitive.indices);
//				std::vector<tinygltf::BufferView>::const_reference transmission_buffer_view = transmission_model.bufferViews.at(transmission_accessor.bufferView);
//				const void* indices = transmission_model.buffers.at(transmission_buffer_view.buffer).data.data() + transmission_buffer_view.byteOffset + transmission_accessor.byteOffset;
//				size_t index_count = transmission_accessor.count;
//
//				if (transmission_model.accessors.at(transmission_primitive.indices).componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
//				{
//					compute_normals<unsigned int>(positions, vertex_count, reinterpret_cast<const unsigned int*>(indices), index_count, normals);
//				}
//				else if (transmission_model.accessors.at(transmission_primitive.indices).componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
//				{
//					compute_normals<unsigned short>(positions, vertex_count, reinterpret_cast<const unsigned short*>(indices), index_count, normals);
//				}
//				else
//				{
//					_ASSERT_EXPR(FALSE, L"This index format is not supported.");
//				}
//
//				buffer_view vertex_buffer_view = {};
//				vertex_buffer_view.format = DXGI_FORMAT_R32G32B32_FLOAT;
//				vertex_buffer_view.stride_in_bytes = sizeof(DirectX::XMFLOAT3);
//				vertex_buffer_view.size_in_bytes = vertex_count * vertex_buffer_view.stride_in_bytes;
//				vertex_buffer_view.buffer = static_cast<int>(buffers.size());
//
//				D3D11_BUFFER_DESC buffer_desc = {};
//				D3D11_SUBRESOURCE_DATA subresource_data = {};
//				buffer_desc.ByteWidth = static_cast<UINT>(vertex_buffer_view.size_in_bytes);
//				buffer_desc.Usage = D3D11_USAGE_DEFAULT;
//				buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//				buffer_desc.CPUAccessFlags = 0;
//				buffer_desc.MiscFlags = 0;
//				buffer_desc.StructureByteStride = 0;
//				subresource_data.pSysMem = normals;
//				subresource_data.SysMemPitch = 0;
//				subresource_data.SysMemSlicePitch = 0;
//				hr = device->CreateBuffer(&buffer_desc, &subresource_data, buffers.emplace_back().ReleaseAndGetAddressOf());
//				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//				primitive.vertex_buffer_views.insert(std::make_pair("NORMAL", vertex_buffer_view));
//			}
//			// If this primitive does not have tangents, calculate them.
//			const bool has_normal = materials.at(primitive.material).data.normal_texture.index > -1;
//			if (has_normal && primitive.vertex_buffer_views.find("TANGENT") == primitive.vertex_buffer_views.end() && primitive.vertex_buffer_views.find("TEXCOORD_0") != primitive.vertex_buffer_views.end())
//			{
//				std::tuple<const void*, size_t> tuple_value;
//
//				tuple_value = addressof("POSITION");
//				const DirectX::XMFLOAT3* positions = reinterpret_cast<const DirectX::XMFLOAT3*>(std::get<0>(tuple_value));
//				const size_t vertex_count = std::get<1>(tuple_value);
//
//				const DirectX::XMFLOAT3* normals = {};
//				if (transmission_primitive.attributes.find("NORMAL") == transmission_primitive.attributes.end())
//				{
//					_ASSERT_EXPR(extra_normals.size() > 0, L"");
//					normals = extra_normals.data();
//				}
//				else
//				{
//					tuple_value = addressof("NORMAL");
//					normals = reinterpret_cast<const DirectX::XMFLOAT3*>(std::get<0>(tuple_value));
//					_ASSERT_EXPR(vertex_count == std::get<1>(tuple_value), L"");
//				}
//
//				tuple_value = addressof("TEXCOORD_0");
//				const DirectX::XMFLOAT2* texcoords = reinterpret_cast<const DirectX::XMFLOAT2*>(std::get<0>(tuple_value));
//				_ASSERT_EXPR(vertex_count == std::get<1>(tuple_value), L"");
//
//				_ASSERT_EXPR(transmission_primitive.indices > -1, L"An index buffer is needed for this process.");
//				std::vector<tinygltf::Accessor>::const_reference transmission_accessor{ transmission_model.accessors.at(transmission_primitive.indices) };
//				std::vector<tinygltf::BufferView>::const_reference transmission_buffer_view{ transmission_model.bufferViews.at(transmission_accessor.bufferView) };
//				const void* indices = transmission_model.buffers.at(transmission_buffer_view.buffer).data.data() + transmission_buffer_view.byteOffset + transmission_accessor.byteOffset;
//				size_t index_count = transmission_accessor.count;
//
//				std::vector<DirectX::XMFLOAT4> tangents;
//				tangents.resize(vertex_count);
//				if (transmission_model.accessors.at(transmission_primitive.indices).componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
//				{
//					compute_tangents<unsigned int>(positions, normals, texcoords, vertex_count, reinterpret_cast<const unsigned int*>(indices), index_count, tangents.data());
//				}
//				else if (transmission_model.accessors.at(transmission_primitive.indices).componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
//				{
//					compute_tangents<unsigned short>(positions, normals, texcoords, vertex_count, reinterpret_cast<const unsigned short*>(indices), index_count, tangents.data());
//				}
//				else
//				{
//					_ASSERT_EXPR(FALSE, L"This index format is not supported.");
//				}
//
//				buffer_view vertex_buffer_view;
//				vertex_buffer_view.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//				vertex_buffer_view.stride_in_bytes = sizeof(DirectX::XMFLOAT4);
//				vertex_buffer_view.size_in_bytes = vertex_count * vertex_buffer_view.stride_in_bytes;
//				vertex_buffer_view.buffer = static_cast<int>(buffers.size());
//
//				D3D11_BUFFER_DESC buffer_desc = {};
//				D3D11_SUBRESOURCE_DATA subresource_data = {};
//				buffer_desc.ByteWidth = static_cast<UINT>(vertex_buffer_view.size_in_bytes);
//				buffer_desc.Usage = D3D11_USAGE_DEFAULT;
//				buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//				buffer_desc.CPUAccessFlags = 0;
//				buffer_desc.MiscFlags = 0;
//				buffer_desc.StructureByteStride = 0;
//				subresource_data.pSysMem = tangents.data();
//				subresource_data.SysMemPitch = 0;
//				subresource_data.SysMemSlicePitch = 0;
//				hr = device->CreateBuffer(&buffer_desc, &subresource_data, buffers.emplace_back().ReleaseAndGetAddressOf());
//				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//				primitive.vertex_buffer_views.insert(std::make_pair("TANGENT", vertex_buffer_view));
//			}
//#endif
//
//#if 1
//			_ASSERT_EXPR(primitive.vertex_buffer_views.find("POSITION") != primitive.vertex_buffer_views.end(), L"POSITION is required for our vertex attributes.");
//#endif
//			// Add dummy attributes if any are missing.
//			const std::unordered_map<std::string, buffer_view> attributes =
//			{
//				{ "POSITION", { DXGI_FORMAT_R32G32B32_FLOAT } },
//				{ "NORMAL", { DXGI_FORMAT_R32G32B32_FLOAT } },
//				{ "TANGENT", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
//				{ "TEXCOORD_0", { DXGI_FORMAT_R32G32_FLOAT } },
//				{ "TEXCOORD_1", { DXGI_FORMAT_R32G32_FLOAT } },
//				{ "JOINTS_0", { DXGI_FORMAT_R16G16B16A16_UINT } },
//				{ "WEIGHTS_0", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
//			};
//			for (std::unordered_map<std::string, buffer_view>::const_reference attribute : attributes)
//			{
//				if (primitive.vertex_buffer_views.find(attribute.first) == primitive.vertex_buffer_views.end())
//				{
//					primitive.vertex_buffer_views.insert(std::make_pair(attribute.first, attribute.second));
//				}
//			}
//		}
//	}
//
//	// Rebuild bounding boxes (min_value / man_value) computationally.
//	for (decltype(nodes)::reference node : nodes)
//	{
//		if (node.mesh > -1)
//		{
//			const mesh& mesh = meshes.at(node.mesh);
//
//			XMMATRIX transform = DirectX::XMLoadFloat4x4(&node.global_transform);
//			for (std::vector<primitive>::const_reference primitive : mesh.primitives)
//			{
//				XMVECTOR max_value = XMLoadFloat3(&primitive.max_value);
//				XMVECTOR min_value = XMLoadFloat3(&primitive.min_value);
//				compute_transformed_bounds(min_value, max_value, transform);
//
//				XMStoreFloat3(&node.max_value, XMVectorMax(XMLoadFloat3(&node.max_value), max_value));
//				XMStoreFloat3(&node.min_value, XMVectorMin(XMLoadFloat3(&node.min_value), min_value));
//			}
//			DirectX::XMStoreFloat3(&max_value, XMVectorMax(XMLoadFloat3(&max_value), XMLoadFloat3(&node.max_value)));
//			DirectX::XMStoreFloat3(&min_value, XMVectorMin(XMLoadFloat3(&min_value), XMLoadFloat3(&node.min_value)));
//		}
//	}
//}
//
//int skeletal_mesh::draw(ID3D11DeviceContext* immediate_context, draw_pass pass, const DirectX::XMFLOAT4X4& transform, const DirectX::XMFLOAT4& color, size_t scene_index)
//{
//	return draw(immediate_context, pass, transform, nodes, color, scene_index);
//}
//int skeletal_mesh::draw(ID3D11DeviceContext* immediate_context, draw_pass pass, const DirectX::XMFLOAT4X4& transform, const std::vector<node>& animated_nodes, const DirectX::XMFLOAT4& color, size_t scene_index)
//{
//	_ASSERT_EXPR(animated_nodes.size() == nodes.size(), L"");
//
//	int drawcall_count = 0;
//
//	using namespace DirectX;
//	XMMATRIX world_transform = XMLoadFloat4x4(&transform);
//
//	immediate_context->VSSetShader(skeletal_mesh_vs.Get(), nullptr, 0);
//	immediate_context->PSSetShader(skeletal_mesh_ps.Get(), nullptr, 0);
//	immediate_context->IASetInputLayout(input_layout.Get());
//
//	// The slot number of 'material_constants' are defined in 'material.hlsli'.
//	const int material_constants_slot = 0;
//	immediate_context->PSSetShaderResources(material_constants_slot, 1, material_resource_view.GetAddressOf());
//
//	std::function<void(int)> traverse = [&](int node_index) {
//		const node& node = animated_nodes.at(node_index);
//		XMMATRIX global_transform = XMLoadFloat4x4(&node.global_transform);
//
//		if (node.skin > -1)
//		{
//			const skin& skin = skins.at(node.skin);
//			_ASSERT_EXPR(skin.joints.size() <= _max_joints, L"The size of the joint array is insufficient, please expand it.");
//			for (size_t joint_index = 0; joint_index < skin.joints.size(); ++joint_index)
//			{
//				XMStoreFloat4x4(&joint_constants->data.matrices[joint_index],
//					XMLoadFloat4x4(&skin.inverse_bind_matrices.at(joint_index)) *
//					XMLoadFloat4x4(&animated_nodes.at(skin.joints.at(joint_index)).global_transform) *
//					XMMatrixInverse(NULL, global_transform)
//				);
//			}
//			joint_constants->activate(immediate_context, _joint_slot, cb_usage::v);
//		}
//		if (node.mesh > -1)
//		{
//			const mesh& mesh = meshes.at(node.mesh);
//			for (std::vector<primitive>::const_reference primitive : mesh.primitives)
//			{
//				const material& material = materials.at(primitive.material < 0 ? materials.size() - 1 : primitive.material);
//#if 1
//				bool permitted = false;
//				switch (pass)
//				{
//				case draw_pass::opaque:
//					permitted = material.has_opaque();
//					break;
//				case draw_pass::transmission:
//					permitted = material.has_transmission();
//					break;
//				}
//				if (!permitted)
//				{
//					continue;
//				}
//#endif
//
//				ID3D11Buffer* vertex_buffers[] =
//				{
//					primitive.vertex_buffer_views.at("POSITION").buffer > -1 ? buffers.at(primitive.vertex_buffer_views.at("POSITION").buffer).Get() : NULL,
//					primitive.vertex_buffer_views.at("NORMAL").buffer > -1 ? buffers.at(primitive.vertex_buffer_views.at("NORMAL").buffer).Get() : NULL,
//					primitive.vertex_buffer_views.at("TANGENT").buffer > -1 ? buffers.at(primitive.vertex_buffer_views.at("TANGENT").buffer).Get() : NULL,
//					primitive.vertex_buffer_views.at("TEXCOORD_0").buffer > -1 ? buffers.at(primitive.vertex_buffer_views.at("TEXCOORD_0").buffer).Get() : NULL,
//					primitive.vertex_buffer_views.at("TEXCOORD_1").buffer > -1 ? buffers.at(primitive.vertex_buffer_views.at("TEXCOORD_1").buffer).Get() : NULL,
//					primitive.vertex_buffer_views.at("JOINTS_0").buffer > -1 ? buffers.at(primitive.vertex_buffer_views.at("JOINTS_0").buffer).Get() : NULL,
//					primitive.vertex_buffer_views.at("WEIGHTS_0").buffer > -1 ? buffers.at(primitive.vertex_buffer_views.at("WEIGHTS_0").buffer).Get() : NULL,
//				};
//				UINT strides[] =
//				{
//					static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").stride_in_bytes),
//					static_cast<UINT>(primitive.vertex_buffer_views.at("NORMAL").stride_in_bytes),
//					static_cast<UINT>(primitive.vertex_buffer_views.at("TANGENT").stride_in_bytes),
//					static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_0").stride_in_bytes),
//					static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_1").stride_in_bytes),
//					static_cast<UINT>(primitive.vertex_buffer_views.at("JOINTS_0").stride_in_bytes),
//					static_cast<UINT>(primitive.vertex_buffer_views.at("WEIGHTS_0").stride_in_bytes),
//				};
//				UINT offsets[_countof(vertex_buffers)] = { 0 };
//				immediate_context->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
//
//				// The slot numbers of 'primitive_textures' are defined in 'material.hlsli'.
//				const int texture_indices[] =
//				{
//					material.data.pbr_metallic_roughness.basecolor_texture.index,
//					material.data.pbr_metallic_roughness.metallic_roughness_texture.index,
//					material.data.normal_texture.index,
//					material.data.emissive_texture.index,
//					material.data.occlusion_texture.index,
//					material.data.khr_materials_pbr_specular_glossiness.diffuse_texture.index,
//					material.data.khr_materials_pbr_specular_glossiness.specular_glossiness_texture.index,
//					material.data.khr_materials_sheen.sheen_color_texture.index,
//					material.data.khr_materials_sheen.sheen_roughness_texture.index,
//					material.data.khr_materials_specular.specular_texture.index,
//					material.data.khr_materials_specular.specular_color_texture.index,
//					material.data.khr_materials_clearcoat.clearcoat_texture.index,
//					material.data.khr_materials_clearcoat.clearcoat_roughness_texture.index,
//					material.data.khr_materials_clearcoat.clearcoat_normal_texture.index,
//					material.data.khr_materials_transmission.transmission_texture.index,
//					material.data.khr_materials_volume.thickness_texture.index,
//				};
//				ID3D11ShaderResourceView* null_shader_resource_view = {};
//				std::vector<ID3D11ShaderResourceView*> shader_resource_views(_countof(texture_indices));
//				for (int texture_index = 0; texture_index < shader_resource_views.size(); ++texture_index)
//				{
//					shader_resource_views.at(texture_index) = texture_indices[texture_index] > -1 ? texture_resource_views.at(textures.at(texture_indices[texture_index]).source).Get() : null_shader_resource_view;
//				}
//				immediate_context->PSSetShaderResources(1, static_cast<UINT>(shader_resource_views.size()), shader_resource_views.data());
//
//				XMStoreFloat4x4(&primitive_constants->data.transform, global_transform * world_transform);
//				primitive_constants->data.color = color;
//				primitive_constants->data.material = primitive.material < 0 ? static_cast<int>(materials.size() - 1) : primitive.material;
//				primitive_constants->data.skin = node.skin;
//				primitive_constants->activate(immediate_context, _primitive_slot, cb_usage::vp);
//
//				immediate_context->IASetPrimitiveTopology(primitive.topology);
//
//				drawcall_count++;
//				if (primitive.index_buffer_view.count() > 0)
//				{
//					immediate_context->IASetIndexBuffer(buffers.at(primitive.index_buffer_view.buffer).Get(), primitive.index_buffer_view.format, 0);
//					immediate_context->DrawIndexed(static_cast<UINT>(primitive.index_count), static_cast<UINT>(primitive.index_location), 0);
//				}
//				else
//				{
//					immediate_context->Draw(static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").count()), 0);
//				}
//			}
//		}
//		for (decltype(node.children)::value_type child_index : node.children)
//		{
//			traverse(child_index);
//		}
//	};
//	const scene& scene = scenes.at(scene_index);
//	for (decltype(scene.nodes)::value_type node_index : scene.nodes)
//	{
//		traverse(node_index);
//	}
//	return drawcall_count;
//}
//int skeletal_mesh::cast_shadow(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const std::vector<node>& animated_nodes, size_t scene_index)
//{
//	_ASSERT_EXPR(animated_nodes.size() == nodes.size(), L"");
//
//	int drawcall_count = 0;
//
//	using namespace DirectX;
//	XMMATRIX world_transform = XMLoadFloat4x4(&world);
//
//	for (int pass = 0; pass < 2; ++pass)
//	{
//		std::function<void(int)> traverse = [&](int node_index) {
//			const node& node = animated_nodes.at(node_index);
//			XMMATRIX global_transform = XMLoadFloat4x4(&node.global_transform);
//
//			if (node.skin > -1)
//			{
//				const skin& skin = skins.at(node.skin);
//				_ASSERT_EXPR(skin.joints.size() <= _max_joints, L"The size of the joint array is insufficient, please expand it.");
//				for (size_t joint_index = 0; joint_index < skin.joints.size(); ++joint_index)
//				{
//					XMStoreFloat4x4(&joint_constants->data.matrices[joint_index],
//						XMLoadFloat4x4(&skin.inverse_bind_matrices.at(joint_index)) *
//						XMLoadFloat4x4(&animated_nodes.at(skin.joints.at(joint_index)).global_transform) *
//						XMMatrixInverse(NULL, global_transform)
//					);
//				}
//				joint_constants->activate(immediate_context, _joint_slot, cb_usage::v);
//			}
//			if (node.mesh > -1)
//			{
//				const mesh& mesh = meshes.at(node.mesh);
//				for (std::vector<primitive>::const_reference primitive : mesh.primitives)
//				{
//					const material& material = materials.at(primitive.material < 0 ? materials.size() - 1 : primitive.material);
//
//					bool permitted = false;
//					switch (pass)
//					{
//					case 0: // opaque
//						permitted = material.has_opaque();
//						break;
//					case 1: // transmission
//						permitted = material.has_transmission();
//						break;
//					}
//					if (!permitted)
//					{
//						continue;
//					}
//
//					if (pass == 0)
//					{
//						immediate_context->VSSetShader(csm_opaque_skeletal_mesh_vs.Get(), NULL, 0);
//						immediate_context->GSSetShader(csm_opaque_gs.Get(), NULL, 0);
//						immediate_context->PSSetShader(NULL, NULL, 0);
//						immediate_context->IASetInputLayout(csm_opaque_input_layout.Get());
//
//						ID3D11Buffer* vertex_buffers[] =
//						{
//							primitive.vertex_buffer_views.at("POSITION").buffer > -1 ? buffers.at(primitive.vertex_buffer_views.at("POSITION").buffer).Get() : NULL,
//							primitive.vertex_buffer_views.at("JOINTS_0").buffer > -1 ? buffers.at(primitive.vertex_buffer_views.at("JOINTS_0").buffer).Get() : NULL,
//							primitive.vertex_buffer_views.at("WEIGHTS_0").buffer > -1 ? buffers.at(primitive.vertex_buffer_views.at("WEIGHTS_0").buffer).Get() : NULL,
//						};
//						UINT strides[] =
//						{
//							static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").stride_in_bytes),
//							static_cast<UINT>(primitive.vertex_buffer_views.at("JOINTS_0").stride_in_bytes),
//							static_cast<UINT>(primitive.vertex_buffer_views.at("WEIGHTS_0").stride_in_bytes),
//						};
//						UINT offsets[_countof(vertex_buffers)] = { 0 };
//						immediate_context->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
//					}
//					else
//					{
//						immediate_context->VSSetShader(csm_transparent_skeletal_mesh_vs.Get(), NULL, 0);
//						immediate_context->GSSetShader(csm_transparent_gs.Get(), NULL, 0);
//						immediate_context->PSSetShader(csm_transparent_ps.Get(), NULL, 0);
//						immediate_context->IASetInputLayout(csm_transparent_input_layout.Get());
//
//						ID3D11Buffer* vertex_buffers[4] =
//						{
//							primitive.vertex_buffer_views.at("POSITION").buffer > -1 ? buffers.at(primitive.vertex_buffer_views.at("POSITION").buffer).Get() : NULL,
//							primitive.vertex_buffer_views.at("JOINTS_0").buffer > -1 ? buffers.at(primitive.vertex_buffer_views.at("JOINTS_0").buffer).Get() : NULL,
//							primitive.vertex_buffer_views.at("WEIGHTS_0").buffer > -1 ? buffers.at(primitive.vertex_buffer_views.at("WEIGHTS_0").buffer).Get() : NULL,
//							NULL,
//						};
//						UINT strides[4] =
//						{
//							static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").stride_in_bytes),
//							static_cast<UINT>(primitive.vertex_buffer_views.at("JOINTS_0").stride_in_bytes),
//							static_cast<UINT>(primitive.vertex_buffer_views.at("WEIGHTS_0").stride_in_bytes),
//							0,
//						};
//						int texcoord = material.data.pbr_metallic_roughness.basecolor_texture.texcoord;
//						switch (texcoord)
//						{
//						case 0:
//							vertex_buffers[3] = buffers.at(primitive.vertex_buffer_views.at("TEXCOORD_0").buffer).Get();
//							strides[3] = static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_0").stride_in_bytes);
//							break;
//						case 1:
//							vertex_buffers[3] = buffers.at(primitive.vertex_buffer_views.at("TEXCOORD_1").buffer).Get();
//							strides[3] = static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_1").stride_in_bytes);
//							break;
//						}
//
//						UINT offsets[_countof(vertex_buffers)] = { 0 };
//						immediate_context->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
//
//						ID3D11ShaderResourceView* texture_resource_view = NULL;
//						int basecolor_texture_index = material.data.pbr_metallic_roughness.basecolor_texture.index;
//						if (basecolor_texture_index > -1)
//						{
//							int image = textures.at(basecolor_texture_index).source;
//							if (image > -1)
//							{
//								texture_resource_view = texture_resource_views.at(image).Get();
//								immediate_context->PSSetShaderResources(1, 1, &texture_resource_view);
//							}
//						}
//					}
//
//					XMStoreFloat4x4(&primitive_constants->data.transform, global_transform * world_transform);
//					primitive_constants->data.skin = node.skin;
//					primitive_constants->data.start_instance_location = 0;
//					primitive_constants->activate(immediate_context, _primitive_slot, cb_usage::v);
//
//					immediate_context->IASetPrimitiveTopology(primitive.topology);
//
//					drawcall_count++;
//					if (primitive.index_buffer_view.count() > 0)
//					{
//						immediate_context->IASetIndexBuffer(buffers.at(primitive.index_buffer_view.buffer).Get(), primitive.index_buffer_view.format, 0);
//						immediate_context->DrawIndexedInstanced(static_cast<UINT>(primitive.index_count), 4, static_cast<UINT>(primitive.index_location), 0, 0);
//					}
//					else
//					{
//						immediate_context->DrawInstanced(static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").count()), 4, 0, 0);
//					}
//				}
//			}
//			for (decltype(node.children)::value_type child_index : node.children)
//			{
//				traverse(child_index);
//			}
//		};
//		const scene& scene = scenes.at(scene_index);
//		for (decltype(scene.nodes)::value_type node_index : scene.nodes)
//		{
//			traverse(node_index);
//		}
//	}
//
//	immediate_context->VSSetShader(NULL, NULL, 0);
//	immediate_context->PSSetShader(NULL, NULL, 0);
//	immediate_context->GSSetShader(NULL, NULL, 0);
//	immediate_context->IASetInputLayout(NULL);
//
//	return drawcall_count;
//}