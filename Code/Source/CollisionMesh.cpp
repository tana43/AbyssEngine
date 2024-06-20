#include "CollisionMesh.h"
#include "Actor.h"
#include "AssetManager.h"
#include "Engine.h"

using namespace AbyssEngine;

void CollisionMesh::Initialize(const std::shared_ptr<Actor>& actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();

    //モデル読み込み
    const auto it = Engine::assetManager_->cacheCollisionMesh_.find(filePath_);
    if (it != Engine::assetManager_->cacheCollisionMesh_.end())
    {
        mesh_ = it->second;
    }
    else
    {
        //一度も読み込まれていないモデルなら新たに読み込み、アセットマネージャーに登録
        mesh_ = std::make_shared<GltfCollisionMesh>(filePath_.c_str());
        Engine::assetManager_->cacheCollisionMesh_[filePath_] = mesh_;
    }
}
