#include "CollisionMesh.h"

#include <stack>
#include <functional>
#include <fbxsdk.h>

#include "GeometricSubstance.h"

using namespace DirectX;
using namespace AbyssEngine;

CollisionMesh::CollisionMesh(ID3D11Device* device, const std::string& fileName, bool triangulate)
{
	//	Gltf読み込み
	{
		tinygltf::TinyGLTF tinyGltf;
		tinyGltf.SetImageLoader(NullLoadImageData, nullptr);

		tinygltf::Model gltfModel;
		std::string error, warning;
		bool succeeded{ false };

		if (fileName.find(".glb") != std::string::npos)
		{
			succeeded = tinyGltf.LoadBinaryFromFile(&gltfModel, &error, &warning, fileName.c_str());
		}
		else if (fileName.find(".gltf") != std::string::npos)
		{
			succeeded = tinyGltf.LoadASCIIFromFile(&gltfModel, &error, &warning, fileName.c_str());
		}
		assert(succeeded && "Failed to load glTF file");

		std::stack<DirectX::XMFLOAT4X4> parentGlobalTransforms;
		std::function<void(int)> traverse = [&](int nodeIndex)
		{
			decltype(gltfModel.nodes)::value_type gltfNode = gltfModel.nodes.at(nodeIndex);

			DirectX::XMFLOAT4X4 globalTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			if (!gltfNode.matrix.empty())
			{
				DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
				for (size_t row = 0; row < 4; row++)
				{
					for (size_t column = 0; column < 4; column++)
					{
						transform(row, column) = static_cast<float>(gltfNode.matrix.at(4 * row + column));
					}
				}
				DirectX::XMStoreFloat4x4(&globalTransform, DirectX::XMLoadFloat4x4(&transform) * DirectX::XMLoadFloat4x4(&parentGlobalTransforms.top()));
			}
			else
			{
				DirectX::XMMATRIX S = gltfNode.scale.size() > 0 ? DirectX::XMMatrixScaling(static_cast<float>(gltfNode.scale.at(0)), static_cast<float>(gltfNode.scale.at(1)), static_cast<float>(gltfNode.scale.at(2))) : DirectX::XMMatrixIdentity();
				DirectX::XMMATRIX R = gltfNode.rotation.size() > 0 ? DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(static_cast<float>(gltfNode.rotation.at(0)), static_cast<float>(gltfNode.rotation.at(1)), static_cast<float>(gltfNode.rotation.at(2)), static_cast<float>(gltfNode.rotation.at(3)))) : DirectX::XMMatrixIdentity();
				DirectX::XMMATRIX T = gltfNode.translation.size() > 0 ? DirectX::XMMatrixTranslation(static_cast<float>(gltfNode.translation.at(0)), static_cast<float>(gltfNode.translation.at(1)), static_cast<float>(gltfNode.translation.at(2))) : DirectX::XMMatrixIdentity();
				DirectX::XMStoreFloat4x4(&globalTransform, S * R * T * DirectX::XMLoadFloat4x4(&parentGlobalTransforms.top()));
			}

			if (gltfNode.mesh > -1)
			{
				std::vector<tinygltf::Mesh>::const_reference gltfMesh = gltfModel.meshes.at(gltfNode.mesh);
				decltype(meshes_)::reference mesh = meshes_.emplace_back();
				mesh.name_ = gltfMesh.name;
				for (std::vector<tinygltf::Primitive>::const_reference gltfPrimitive : gltfMesh.primitives)
				{
					std::vector<tinygltf::Material>::const_reference gltfMaterial = gltfModel.materials.at(gltfPrimitive.material);
					decltype(mesh.subsets)::reference subset = mesh.subsets.emplace_back();
					subset.materialName_ = gltfMaterial.name;

					const tinygltf::Accessor&	indexAccessor = gltfModel.accessors.at(gltfPrimitive.indices);
					const tinygltf::BufferView& indexBufferView = gltfModel.bufferViews.at(indexAccessor.bufferView);
					assert(indexAccessor.type == TINYGLTF_TYPE_SCALAR);
					assert(indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT);
					unsigned int* indices = reinterpret_cast<unsigned int*>(gltfModel.buffers.at(indexBufferView.buffer).data.data() + indexBufferView.byteOffset + indexAccessor.byteOffset);

					const tinygltf::Accessor& positionAccessor = gltfModel.accessors.at(gltfPrimitive.attributes.at("POSITION"));
					const tinygltf::BufferView& positionBufferView = gltfModel.bufferViews.at(positionAccessor.bufferView);
					DirectX::XMFLOAT3* positions = reinterpret_cast<DirectX::XMFLOAT3*>(gltfModel.buffers.at(positionBufferView.buffer).data.data() + positionBufferView.byteOffset + positionAccessor.byteOffset);

					for (size_t indexIndex = 0; indexIndex < indexAccessor.count; ++indexIndex)
					{
						unsigned int index = indices[indexIndex];
						DirectX::XMFLOAT3 position = positions[index];
						DirectX::XMStoreFloat3(&position, XMVector3TransformCoord(XMLoadFloat3(&position), DirectX::XMLoadFloat4x4(&globalTransform)));
						subset.positions_.emplace_back(position);
					}
				}
			}
			for (decltype(gltfNode.children)::value_type childIndex : gltfNode.children)
			{
				parentGlobalTransforms.push(globalTransform);
				traverse(childIndex);
				parentGlobalTransforms.pop();
			}
		};
		decltype(gltfModel.scenes)::const_reference gltfScene = gltfModel.scenes.at(0);
		for (decltype(gltfScene.nodes)::value_type rootNode : gltfScene.nodes)
		{
			parentGlobalTransforms.push({ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });
			traverse(rootNode);
			parentGlobalTransforms.pop();
		}
	}

	//	Rebuild bounding box
	for (decltype(meshes_)::reference mesh : meshes_)
	{
		for (decltype(mesh.subsets)::reference subset : mesh.subsets)
		{
			for (decltype(subset.positions_)::reference position : subset.positions_)
			{
				mesh.boundingBox_[0].x = std::min<float>(mesh.boundingBox_[0].x, position.x);
				mesh.boundingBox_[0].y = std::min<float>(mesh.boundingBox_[0].y, position.y);
				mesh.boundingBox_[0].z = std::min<float>(mesh.boundingBox_[0].z, position.z);
				mesh.boundingBox_[1].x = std::max<float>(mesh.boundingBox_[1].x, position.x);
				mesh.boundingBox_[1].y = std::max<float>(mesh.boundingBox_[1].y, position.y);
				mesh.boundingBox_[1].z = std::max<float>(mesh.boundingBox_[1].z, position.z);
			}
		}
	}
}

inline bool IntersectRayAABB(const float rayPos[3], const float rayDirection[3], const float p0[3], const float p1[3])
{
	float tMin = 0;
	float tMax = +FLT_MAX;

	for (size_t a = 0; a < 3; ++a)
	{
		float inverseDirection = 1.0f / rayDirection[a];
		float t0 = (p0[a] - rayPos[a]) * inverseDirection;
		float t1 = (p1[a] - rayPos[a]) * inverseDirection;
		if (inverseDirection < 0.0f)
		{
			std::swap<float>(t0, t1);
		}
		tMin = std::max<float>(t0, tMin);
		tMax = std::min<float>(t1, tMax);

		if (tMax <= tMin)
		{
			return false;
		}
	}
	return true;
}

// The coordinate system of all function arguments is world space.
bool CollisionMesh::Raycast(_In_ DirectX::XMFLOAT3 rayPosition, _In_ DirectX::XMFLOAT3 rayDirection, _In_ const DirectX::XMFLOAT4X4& transform, _Out_ DirectX::XMFLOAT3& intersectionPosition, _Out_ DirectX::XMFLOAT3& intersectionNormal,
	_Out_ std::string& intersectionMesh, _Out_ std::string& intersectionMaterial, _In_ float rayLengthLimit, _In_ bool skipIf) const
{
	//	モデルのワールド変換行列を取得し、逆行列を計算する
	DirectX::XMMATRIX T = DirectX::XMLoadFloat4x4(&transform);
	DirectX::XMMATRIX inverseT = DirectX::XMMatrixInverse(NULL, T);

	//	レイの始点と方向をモデル空間に変換する
	DirectX::XMStoreFloat3(&rayPosition, DirectX::XMVector3TransformCoord(XMLoadFloat3(&rayPosition), inverseT));
	DirectX::XMStoreFloat3(&rayDirection, DirectX::XMVector3TransformNormal(XMLoadFloat3(&rayDirection), inverseT));

	//	レイの始点と方向をVECTORに変換する
	const DirectX::XMVECTOR P = XMLoadFloat3(&rayPosition);
	const DirectX::XMVECTOR D = DirectX::XMVector3Normalize(XMLoadFloat3(&rayDirection));

	//	交差が検出された三角形の数と、最も近い交差点までの距離
	int intersectionCount = 0;
	float closestDistance = FLT_MAX;

	//	各メッシュについて処理を行う
	for (decltype(meshes_)::const_reference mesh : meshes_)
	{
#if 0
		const float* AABBMin = reinterpret_cast<const float*>(&mesh.boundingBox_[0]);
		const float* AABBMax = reinterpret_cast<const float*>(&mesh.boundingBox_[1]);
		if (!IntersectRayAABB(reinterpret_cast<const float*>(&rayPosition), reinterpret_cast<const float*>(&rayDirection), AABBMin, AABBMax))
		{
			continue;
		}
#endif
		//	三角形の法線を計算する
		for (decltype(mesh.subsets)::const_reference subset : mesh.subsets)
		{
			const DirectX::XMFLOAT3* positions = subset.positions_.data();
			const size_t triangleCount = subset.positions_.size() / 3;
			for (size_t triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++)
			{
				const DirectX::XMVECTOR A = XMLoadFloat3(&positions[triangleIndex * 3 + 0]);
				const DirectX::XMVECTOR B = XMLoadFloat3(&positions[triangleIndex * 3 + 1]);
				const DirectX::XMVECTOR C = XMLoadFloat3(&positions[triangleIndex * 3 + 2]);

				const DirectX::XMVECTOR N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(B, A), DirectX::XMVectorSubtract(C, A)));
				const float d = DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(N, A), 0);
				const float denominator{ DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(N, D), 0) };
				
				//	三角形が平行でない場合、交差を検出する
				if (denominator < 0)
				{
					const float numerator = d - DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(N, P), 0);
					const float t = numerator / denominator;

					//	レイが三角形と交差し、かつレイの長さ制限内にある場合
					if (t > 0 && t < rayLengthLimit)
					{
						//	交差点を計算する
						DirectX::XMVECTOR Q = DirectX::XMVectorAdd(P, DirectX::XMVectorScale(D, t));

						//	交差点が三角形の内部にあるかどうかを判定する
						const DirectX::XMVECTOR QA = DirectX::XMVectorSubtract(A, Q);
						const DirectX::XMVECTOR QB = DirectX::XMVectorSubtract(B, Q);
						const DirectX::XMVECTOR QC = DirectX::XMVectorSubtract(C, Q);

						DirectX::XMVECTOR U = DirectX::XMVector3Cross(QB, QC);
						DirectX::XMVECTOR V = DirectX::XMVector3Cross(QC, QA);
						if (DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(U, V), 0) < 0)
						{
							continue;
						}

						DirectX::XMVECTOR W = DirectX::XMVector3Cross(QA, QB);
						if (DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(U, W), 0) < 0)
						{
							continue;
						}
						if (DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(V, W), 0) < 0)
						{
							continue;
						}
						intersectionCount++;

						//	交差点が直前の最短距離よりも近い場合、最短距離と交差情報を更新する
						if (t < closestDistance)
						{
							closestDistance = t;

							XMStoreFloat3(&intersectionPosition, XMVector3TransformCoord(Q, T));
							XMStoreFloat3(&intersectionNormal, DirectX::XMVector3Normalize(XMVector3TransformNormal(N, T)));
							intersectionMesh = mesh.name_;
							intersectionMaterial = subset.materialName_;

							if (skipIf)
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
	return intersectionCount > 0;
}
