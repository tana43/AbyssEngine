#include "StaticMesh.h"
#include "Actor.h"
#include "GltfStaticMesh.h"
#include "Engine.h"
#include "RenderManager.h"
#include "AssetManager.h"

#include "imgui/imgui.h"
#include "ShapeRenderer.h"

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
    const auto& world = transform_->CalcWorldMatrix();

    //�o�E���f�B���O�{�b�N�X�X�V
    DirectX::XMVECTOR MinValue, MaxValue;
    MinValue = DirectX::XMLoadFloat3(&model_->minValue_);
    MaxValue = DirectX::XMLoadFloat3(&model_->maxValue_);
    ComputeTransformedBounds(MinValue,MaxValue, world /** coordinateSystemTransforms[0]*/);
    DirectX::XMStoreFloat3(&minValue_, MinValue);
    DirectX::XMStoreFloat3(&maxValue_, MaxValue);

    boundingBox_ = ConvertToDXBoundingBox(minValue_,maxValue_);
}

void StaticMesh::Render()
{
    //model_->Draw(DrawPass::Opaque,transform_->CalcWorldMatrix());
    model_->Draw(DrawPass::Opaque,transform_->GetWorldMatrix());
    //model_->Draw(DrawPass::Transmission,transform_->GetWorldMatrix());

#if _DEBUG
    //�o�E���f�B���O�{�b�N�X�\��
    Engine::renderManager_->shapeRenderer_->DrawBox(
        boundingBox_.Center,
        Vector3(0,0,0),
        boundingBox_.Extents,
        Vector4(0,1,1,1)
    );
#endif // _DEBUG

}

void StaticMesh::RenderShadow()
{
    model_->CastShadow(transform_->GetWorldMatrix());
}

bool StaticMesh::FrustumCulling(const DirectX::BoundingFrustum& frustum)
{
    DirectX::ContainmentType ret = boundingBox_.Contains(frustum);
    if (ret >= DirectX::INTERSECTS)
    {
        return true;
    }
    return false;
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
