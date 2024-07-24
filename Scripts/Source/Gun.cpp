#include "Gun.h"
#include "Actor.h"
#include "SceneManager.h"
#include "Engine.h"
#include "Bullet.h"
#include "imgui/imgui.h"

#include "DebugRenderer.h"
#include "RenderManager.h"

using namespace AbyssEngine;

void Gun::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    ScriptComponent::Initialize(actor);
}

bool Gun::DrawImGui()
{
    if (ImGui::TreeNode("Gun"))
    {
        if (ImGui::Button("Shot"))
        {
            Shot(transform_->GetForward());
        }

        ImGui::SliderFloat("Rate Timer", &rateTimer_, 0.0f, rateOfFire_);
        ImGui::SliderFloat("RateOfFire", &rateOfFire_, 0.0f, 0.3f);

        ImGui::TreePop();
    }

    return true;
}

void Gun::DrawDebug()
{
#if _DEBUG
    Engine::renderManager_->debugRenderer_->DrawSphere(muzzlePos_, 0.1f, { 1, 1, 1, 1 });
#endif // _DEBUG
}

void Gun::Update()
{
    //発射レート更新
    rateTimer_ -= Time::deltaTime_;
}

void Gun::Shot(AbyssEngine::Vector3 shootingDirection)
{
    //撃つことが可能か
    if (rateTimer_ < 0)
    {
        //弾丸生成
        const auto& bullet = Engine::sceneManager_->GetActiveScene().InstanceActor("Bullet");
        const auto& proj = bullet->AddComponent<Bullet>();
        bullet->GetTransform()->SetPosition(muzzlePos_);

        proj->SetDirection(shootingDirection);

        rateTimer_ = rateOfFire_;
    }
    
    //弾数管理

}
