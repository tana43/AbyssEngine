#include "SphereCollider.h"
#include "Engine.h"
#include "RenderManager.h"
#include "DebugRenderer.h"
#include "Transform.h"
#include "SkeletalMesh.h"

#include <imgui/imgui.h>

using namespace AbyssEngine;

void SphereCollider::Initialize(const std::shared_ptr<Actor>& actor)
{
    Collider::Initialize(actor);
}

void SphereCollider::UpdateWorldMatrix()
{
    //アタッチ先のモデルがある
    if (const auto& model = attachModel_.lock())
    {
        const Matrix boneTransform = model->FindSocket(socketName_.c_str());
        const Matrix worldMatrix = boneTransform * transform_->CalcWorldMatrix();
        transform_->SetWorldMatrix(worldMatrix);
    }
    else //アタッチ先のモデルがない
    {
        //ワールド行列更新
        transform_->CalcWorldMatrix();
    }
}

void SphereCollider::DrawDebug()
{
#if _DEBUG
    Engine::renderManager_->debugRenderer_->DrawSphere(transform_->GetPosition(), radius_, debugColor_);
#endif // _DEBUG
}

void SphereCollider::DrawImGui()
{
    std::string name = "Sphere Collider" + socketName_;
    if (ImGui::TreeNode(name.c_str()))
    {
        ImGui::DragFloat("Radius", &radius_, 0.02f, 0.0f);

        ImGui::ColorPicker4("DebugColor", &debugColor_.x, ImGuiColorEditFlags_PickerHueWheel);

        ImGui::TreePop();
    }
}

bool SphereCollider::IntersectVsSphere(const std::shared_ptr<SphereCollider>& collider,Collision::IntersectionResult* result)
{
    Collision::IntersectSphereVsSphere(transform_->GetPosition(), radius_,
        collider->transform_->GetPosition(), collider->radius_, result);

    if (result->penetration > 0)return true;
    return false;
}

void SphereCollider::AttachModel(const std::shared_ptr<SkeletalMesh>& model, std::string socketName)
{
    attachModel_ = model;
    socketName_ = socketName;
}
