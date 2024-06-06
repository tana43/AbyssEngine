#include "StaticMesh.h"
#include "Actor.h"
#include "GltfStaticMesh.h"
#include "Engine.h"
#include "RenderManager.h"
#include "AssetManager.h"

#include "imgui/imgui.h"

using namespace AbyssEngine;
using namespace std;

void StaticMesh::Initialize(const std::shared_ptr<Actor>& actor)
{
    //�}�l�[�W���[�̓o�^�Ə�����
    actor_ = actor;
    transform_ = actor->GetTransform();

    //���f���ǂݍ���
    const auto it = Engine::assetManager_->cacheStaticMesh_.find(filePath_);
    if (it != Engine::assetManager_->cacheStaticMesh_.end())
    {
        model_ = it->second;
    }
    else
    {
        //��x���ǂݍ��܂�Ă��Ȃ����f���Ȃ�V���ɓǂݍ��݁A�A�Z�b�g�}�l�[�W���[�ɓo�^
        model_ = make_shared<GltfStaticMesh>(filePath_.c_str());
        Engine::assetManager_->cacheStaticMesh_[filePath_] = model_;
    }

    //�����_���[�}�l�[�W���[�ɓo�^
    SetActive(true);

}

bool StaticMesh::DrawImGui()
{
    if (ImGui::TreeNode("StaticMesh"))
    {
        ImGui::Checkbox("Enabled", &enabled_);

        ImGui::TreePop();
    }

    return false;
}

void StaticMesh::RecalculateFrame()
{
    transform_->CalcWorldMatrix();
}

void StaticMesh::Render()
{
    //model_->Draw(DrawPass::Opaque,transform_->CalcWorldMatrix());
    model_->Draw(DrawPass::Opaque,transform_->GetWorldMatrix());
    //model_->Draw(DrawPass::Transmission,transform_->CalcWorldMatrix());
}

void StaticMesh::RenderShadow()
{
    model_->CastShadow(transform_->GetWorldMatrix());
}

void StaticMesh::SetActive(const bool value)
{
    if (value)
    {
        if (!isCalled_)
        {
            Engine::renderManager_->Add(static_pointer_cast<StaticMesh>(shared_from_this()));
            isCalled_ = true;
        }
    }
}
