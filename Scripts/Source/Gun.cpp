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
        ImGui::SliderFloat("Precision", &precision_, 0.0f, 0.3f);

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
    //���˃��[�g�X�V
    rateTimer_ -= Time::deltaTime_;
}

bool Gun::Shot(AbyssEngine::Vector3 shootingDirection)
{
    //�����Ƃ��\��
    if (rateTimer_ < 0)
    {
        //�e�ې���
        const auto& bullet = Engine::sceneManager_->GetActiveScene().InstanceActor("Bullet");
        const auto& proj = bullet->AddComponent<Bullet>();
        bullet->GetTransform()->SetPosition(muzzlePos_);

        //�e�̐��x�𔽉f
        if (precision_ > 0)
        {
            //�e�̐i�s�����𗐐����g���ė���
            const Vector3 forward = shootingDirection;
            const Vector3 right = shootingDirection.Cross(Vector3(0, 1.0f, 0));
            const Vector3 up = forward.Cross(right);
            shootingDirection = shootingDirection + right * (static_cast<float>(rand() % 20 - 10) / 10.0f * precision_);
            shootingDirection = shootingDirection + up * (static_cast<float>(rand() % 20 - 10) / 10.0f * precision_);
            shootingDirection.Normalize();
        }

        proj->SetDirection(shootingDirection);

        rateTimer_ = rateOfFire_;
    }
    else return false;
    
    //TODO:�e���Ǘ�


    return true;
}
