#include "MeshCollider.h"
#include "Actor.h"
#include "AssetManager.h"
#include "Engine.h"
#include "PrimitiveRenderer.h"
#include "RenderManager.h"

using namespace AbyssEngine;

void MeshCollider::Initialize(const std::shared_ptr<Actor>& actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();

    //モデル読み込み
    const auto it = Engine::assetManager_->cacheCollisionMesh_.find(filePath_);
    if (it != Engine::assetManager_->cacheCollisionMesh_.end())
    {
        model_ = it->second;
    }
    else
    {
        //一度も読み込まれていないモデルなら新たに読み込み、アセットマネージャーに登録
        model_ = std::make_shared<GltfCollisionMesh>(DXSystem::GetDevice().Get(), filePath_.c_str());
        Engine::assetManager_->cacheCollisionMesh_[filePath_] = model_;
    }

    model_->Transform(transform_->CalcWorldMatrix());

    for (auto& mesh : model_->meshes_)
    {
        for (auto& subset : mesh.subsets)
        {
            for (size_t index = 0; index < subset.positions_.size(); index += 3)
            {
                Triangle tri;
                tri.positions[0] = subset.positions_.at(index + 0);
                tri.positions[1] = subset.positions_.at(index + 1);
                tri.positions[2] = subset.positions_.at(index + 2);
                DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&tri.positions[0]);
                DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&tri.positions[1]);
                DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&tri.positions[2]);

                //法線を算出
                DirectX::XMVECTOR N = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(B, A), DirectX::XMVectorSubtract(C, A));
                if (DirectX::XMVector3Equal(N, DirectX::XMVectorZero()))
                {
                    // 面を構成できない場合は除外
                    continue;
                }
                N = DirectX::XMVector3Normalize(N);
                DirectX::XMStoreFloat3(&tri.normal, N);

                triangles_.emplace_back(tri);
            }
        }
    }
}

void MeshCollider::Transform()
{
    model_->Transform(transform_->CalcWorldMatrix());
}

void MeshCollider::Transform(const DirectX::XMFLOAT4X4& worldTransform)
{
    model_->Transform(worldTransform);
}

bool MeshCollider::DrawImGui()
{
    return false;
}

void MeshCollider::DrawDebug()
{
#if _DEBUG
    //デバッグ表示
    /*const auto& priRenderer = Engine::renderManager_->primitiveRenderer_;
    for (const auto& triangle : triangles_)
    {
        priRenderer->AddVertex(triangle.positions[0], DirectX::XMFLOAT4(1, 1, 1, 1));
        priRenderer->AddVertex(triangle.positions[1], DirectX::XMFLOAT4(1, 1, 1, 1));
        priRenderer->AddVertex(triangle.positions[2], DirectX::XMFLOAT4(1, 1, 1, 1));
    }*/
#endif // _DEBUG
}
