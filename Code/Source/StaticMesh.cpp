#include "StaticMesh.h"
#include "Actor.h"
#include "GltfStaticMesh.h"
#include "Engine.h"
#include "RenderManager.h"
#include "AssetManager.h"
#include "SkeletalMesh.h"

#include "imgui/imgui.h"
#include "ShapeRenderer.h"

#include <algorithm>

using namespace AbyssEngine;
using namespace std;

int StaticMesh::instanceNum_ = 0;

AbyssEngine::StaticMesh::StaticMesh()
{
    myNum_ = instanceNum_;
    instanceNum_++;
}

void StaticMesh::Initialize(const std::shared_ptr<Actor>& actor)
{
    //マネージャーの登録と初期化
    actor_ = actor;
    transform_ = actor->GetTransform();

    //モデル読み込み
    const auto it = Engine::assetManager_->cacheStaticMesh_.find(filePath_);
    if (it != Engine::assetManager_->cacheStaticMesh_.end())
    {
        model_ = it->second;

    }
    else
    {
        //一度も読み込まれていないモデルなら新たに読み込み、アセットマネージャーに登録
        model_ = make_shared<GltfStaticMesh>(filePath_.c_str());
        Engine::assetManager_->cacheStaticMesh_[filePath_] = model_;
    }

    //レンダラーマネージャーに登録
    SetActive(true);

}

void StaticMesh::DrawImGui()
{
    std::string name = "StaticMesh_" + filePath_ + std::to_string(myNum_);
    
    if (ImGui::TreeNode(name.c_str()))
    {
        ImGui::Checkbox("Enabled", &enabled_);

        ImGui::DragFloat("Emissive Intensity", &model_->primitiveConstants_->data_.emissiveIntensity_, 0.01f, 0.0f);
        ImGui::DragFloat("Image Based Lighting Intensity", &model_->primitiveConstants_->data_.imageBasedLightingIntensity_, 0.01f, 0.0f);
        ImGui::ColorPicker4("Color", &model_->primitiveConstants_->data_.color_.x,ImGuiColorEditFlags_PickerHueWheel);

        if (isAttached_)
        {
            ImGui::DragFloat3("SocketLocation", &socketData_.location_.x, 0.05f);
            ImGui::DragFloat3("SocketRotation", &socketData_.rotation_.x, 0.05f);
            ImGui::DragFloat3("SocketScale", &socketData_.scale_.x, 0.05f);
        }

        ImGui::TreePop();
    }
}

void StaticMesh::RecalculateFrame()
{
    //ワールド行列更新
    //ソケットにアタッチされているならそれに対応した変換行列を算出
    if (isAttached_)
    {
        const auto& animNodes = socketData_.attachedMesh_->GetAnimator()->GetAnimatedNodes();
        std::vector<GeometricSubstance::Node>::const_iterator node;
        //= std::find(animNodes.begin(), animNodes.end(), socketData_.attachedSocketName_.c_str());
        for (auto it = animNodes.begin(); it != animNodes.end();it++)
        {
            if (it->name_ == socketData_.attachedSocketName_.c_str())
            {
                node = it;
                break;
            }
        }
        if (node != animNodes.end())
        {
            //単位を変換するための定数
        	const float toRadian = 0.01745f;
        	const float toMetric = 0.01f;

        	using namespace DirectX;

        	Matrix boneTransform = XMLoadFloat4x4(&node->globalTransform_);
        	socketData_.socketTrasnform_ =
        		XMMatrixScaling(socketData_.scale_.x, socketData_.scale_.y, socketData_.scale_.z)
        		* XMMatrixRotationX(-socketData_.rotation_.x * toRadian)
        		* XMMatrixRotationY(-socketData_.rotation_.y * toRadian)
        		* XMMatrixRotationZ(socketData_.rotation_.z * toRadian)
        		* XMMatrixTranslation(socketData_.location_.x * toMetric, socketData_.location_.y * toMetric, socketData_.location_.z * toMetric);
        	Matrix dxUe5 = XMMatrixSet(-1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1); // LHS Y-Up Z-Forward(DX) -> LHS Z-Up Y-Forward(UE5) 
        	Matrix ue5Gltf = XMMatrixSet(1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1); // LHS Z-Up Y-Forward(UE5) -> RHS Y-Up Z-Forward(glTF) 
            Matrix attachedMatrix = socketData_.attachedMesh_->GetTransform()->GetWorldMatrix();
        	world_ = dxUe5 * socketData_.socketTrasnform_ * ue5Gltf * boneTransform * attachedMatrix;
        }
    }
    else
    {
        world_ = transform_->CalcWorldMatrix();
    }

    //バウンディングボックス更新
    {
        DirectX::XMVECTOR MinValue, MaxValue;
        MinValue = DirectX::XMLoadFloat3(&model_->minValue_);
        MaxValue = DirectX::XMLoadFloat3(&model_->maxValue_);
        ComputeTransformedBounds(MinValue, MaxValue, world_ /** coordinateSystemTransforms[0]*/);
        DirectX::XMStoreFloat3(&minValue_, MinValue);
        DirectX::XMStoreFloat3(&maxValue_, MaxValue);

        boundingBox_ = ConvertToDXBoundingBox(minValue_, maxValue_);
    }
}

void StaticMesh::Render()
{
    //model_->Draw(DrawPass::Opaque,transform_->CalcWorldMatrix());
    model_->Draw(DrawPass::Opaque,world_);
    model_->Draw(DrawPass::Transmission,world_);

#if _DEBUG
    //バウンディングボックス表示
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
    model_->CastShadow(world_);
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

bool StaticMesh::ShadowCulling(const DirectX::BoundingBox& box)
{
    DirectX::ContainmentType ret = boundingBox_.Contains(box);
    if (ret >= DirectX::INTERSECTS)
    {
        return true;
    }
    return false;
}

void StaticMesh::SetEmissiveIntensity(const float& intensity)
{
    model_->primitiveConstants_->data_.emissiveIntensity_ = intensity;
}

void StaticMesh::SetIBLIntensity(const float& intensity)
{
    model_->primitiveConstants_->data_.imageBasedLightingIntensity_ = intensity;
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
