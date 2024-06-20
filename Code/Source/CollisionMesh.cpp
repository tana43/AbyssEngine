#include "CollisionMesh.h"
#include "Actor.h"
#include "AssetManager.h"
#include "Engine.h"

using namespace AbyssEngine;

void CollisionMesh::Initialize(const std::shared_ptr<Actor>& actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();

    //���f���ǂݍ���
    const auto it = Engine::assetManager_->cacheCollisionMesh_.find(filePath_);
    if (it != Engine::assetManager_->cacheCollisionMesh_.end())
    {
        mesh_ = it->second;
    }
    else
    {
        //��x���ǂݍ��܂�Ă��Ȃ����f���Ȃ�V���ɓǂݍ��݁A�A�Z�b�g�}�l�[�W���[�ɓo�^
        mesh_ = std::make_shared<GltfCollisionMesh>(filePath_.c_str());
        Engine::assetManager_->cacheCollisionMesh_[filePath_] = mesh_;
    }
}
