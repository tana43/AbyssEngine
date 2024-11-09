#include "Gun.h"
#include "Actor.h"
#include "SceneManager.h"
#include "Engine.h"
#include "Bullet.h"
#include "Beam.h"
#include "BillboardRenderer.h"

#include "imgui/imgui.h"

#include "DebugRenderer.h"
#include "RenderManager.h"

#include "GameCollider.h"

using namespace AbyssEngine;

void Gun::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    ScriptComponent::Initialize(actor);

    //マズルフラッシュ
    muzzleFlashComponent_ = actor->AddComponent<BillboardRenderer>("./Assets/Effects/Texture/Explosion_02.png");
    muzzleFlashComponent_->SetVisibility(false);
    muzzleFlashComponent_->SetScale(0.5f);
    beamMuzzleFlashComponent_ = actor->AddComponent<BillboardRenderer>("./Assets/Effects/Texture/ElectricalDischarge.png");
    beamMuzzleFlashComponent_->SetVisibility(false);
    beamMuzzleFlashComponent_->SetScale(0.87f);

    //マズルフラッシュ(ParticleEmitter)
    particleEmitter_ = actor->AddComponent<ComputeParticleEmitter>();
    ComputeParticleEmitter::EmitParameter param;
    param.emitNum_ = 30;
    param.texType_ = 0;
    param.lifespan_ = 0.35f;
    param.lifespanAmplitude_ = 0.1f;
    //param.rotationVelocityAmplitude_ = { 0.0f,0.0f,360.0f };
    param.color_ = {0,0.2f,1.0f,1.0f};
    param.colorAmplitud_ = {0,0,1.0f,0};
    param.scaleInit_ = { 0.05f,0.05f,0.05f };
    param.intensity_ = 60.0f;
    param.accelerationAmplitud_ = { 30.0f,30.0f,30.0f };
    particleEmitter_->SetEmitParamater(param);
    particleEmitter_->SetUseTransform(false);
}

void Gun::DrawImGui()
{
    if (ImGui::TreeNode("Gun"))
    {
        if (ImGui::Button("Shot"))
        {
            Shot(transform_->GetForward());
        }

        static int bulletType = 0;
        ImGui::SliderInt("Bullet Type", &bulletType, 0, static_cast<int>(BulletType::Max) - 1);
        if (ImGui::Button("Set Bullet Type"))
        {
            bulletType_ = static_cast<BulletType>(bulletType);
        }

        ImGui::DragFloat3("MuzzlePos", &muzzlePos_.x, 0.1f);

        {
            ImGui::SliderFloat("Rate Timer", &rateTimer_, 0.0f, rateOfFire_);
            ImGui::SliderFloat("RateOfFire", &rateOfFire_, 0.0f, 0.3f);
            ImGui::SliderFloat("Precision", &precision_, 0.0f, 0.3f);
            ImGui::DragFloat("Bullet Speed", &bulletSpeed_, 0.1f);
            ImGui::DragFloat("Lifespan", &bulletLifespan_,0.01f);

        }


        if (ImGui::TreeNode("Beam"))
        {
            ImGui::DragFloat("Beam Width", &beamWidth_, 0.1f, 0.0f);
            ImGui::DragFloat("Beam Billboard Size", &beamScale_, 0.1f);
            ImGui::ColorEdit4("Beam Color", &beamColor_.x, ImGuiColorEditFlags_PickerHueWheel);
            ImGui::ColorEdit4("Beam Particle Color", &beamParticleColor_.x, ImGuiColorEditFlags_PickerHueWheel);
            ImGui::DragFloat("Brightness", &beamIntensity_, 0.01f);

            ImGui::DragFloat("Particle Speed", &particleSpeed_, 0.01f);
            ImGui::DragFloat("Particle Amplitude Speed", &particleAmplitudeSpeed_, 0.01f);

            ImGui::Checkbox("MuzzleFlash Effect", &enableMuzzleFlashParticleEffect_);
            ImGui::Checkbox("Homing", &isHoming_);

            ImGui::DragFloat("Homing Strength",&homingStrength_,0.01f);

            ImGui::TreePop();
        }

        

        ImGui::TreePop();
    }
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
    rateTimer_ -= actor_->GetDeltaTime();

    //マズルフラッシュエフェクトの更新
    UpdateFlashEffect();
    UpdateFlashParticleEffect();

    flashLifespan_ += actor_->GetDeltaTime();
}

bool Gun::Shot(AbyssEngine::Vector3 shootingDirection)
{
    //撃つことが可能か
    if (!activeRateOfFire_ || rateTimer_ < 0)
    {
        //銃の精度を反映
        if (precision_ > 0)
        {
            //弾の進行方向を乱数を使って乱す
            const Vector3 forward = shootingDirection;
            const Vector3 right = shootingDirection.Cross(Vector3(0, 1.0f, 0));
            const Vector3 up = forward.Cross(right);
            shootingDirection = shootingDirection + right * (static_cast<float>(rand() % 20 - 10) / 10.0f * precision_);
            shootingDirection = shootingDirection + up * (static_cast<float>(rand() % 20 - 10) / 10.0f * precision_);
            shootingDirection.Normalize();
        }

        //弾丸生成
        const auto& bullet = Engine::sceneManager_->GetActiveScene().InstanceActor("Bullet");
        switch (bulletType_)
        {
        case Gun::BulletType::Bullet:
        {
            const auto& proj = bullet->AddComponent<Bullet>();
            //弾丸の設定
            bullet->GetTransform()->SetPosition(muzzlePos_);
            proj->SetRadius(bulletRadius_);
            proj->GetAtkCollider()->ReplaceTag(colliderTag_);
            proj->SetDirection(shootingDirection);
            proj->SetSpeed(bulletSpeed_);
            proj->SetLifespan(bulletLifespan_);

            //エフェクト設定
            muzzleFlashComponent_->SetVisibility(true);
            flashLifespan_ = 0.0f;
            muzzleFlashComponent_->SetRotationZ(Math::RandomRange(0.0f, 360.0f));

            break;
        }
        case Gun::BulletType::Beam:
        {
            const auto& proj = bullet->AddComponent<Beam>();
            //弾丸の設定
            bullet->GetTransform()->SetPosition(muzzlePos_);
            proj->SetRadius(bulletRadius_);
            proj->GetAtkCollider()->ReplaceTag(colliderTag_);
            proj->SetDirection(shootingDirection);
            proj->SetColor(beamColor_);
            proj->SetWidth(beamWidth_);
            proj->GetTransform()->SetScaleFactor(beamScale_);
            proj->SetSpeed(bulletSpeed_);
            proj->SetHomingStrength(homingStrength_);
            proj->SetIsHoming(isHoming_);
            proj->SetTargetTag(targetTag_);
            proj->SetIntensity(beamIntensity_);
            proj->GetParticleEmitParameter().color_ = beamParticleColor_;
            proj->SetLifespan(bulletLifespan_);

            //エフェクト設定
            beamMuzzleFlashComponent_->SetVisibility(true);
            flashLifespan_ = 0.0f;
            beamMuzzleFlashComponent_->SetRotationZ(Math::RandomRange(0.0f, 360.0f));

            //パーティクル設定
            auto emitParam = particleEmitter_->GetEmitParamter();
            emitParam.velocity_ = shootingDirection * particleSpeed_;

            //射撃方向から見た右ベクトルと上ベクトルを算出し、拡散方向を指定する
            const Vector3 right = shootingDirection.Cross(Vector3::Up);
            const Vector3 up = shootingDirection.Cross(right);
            emitParam.positionAmplitude_ = shootingDirection * 0.1f;
            emitParam.velocityAmplitude_ = right * particleAmplitudeSpeed_ + up * particleAmplitudeSpeed_ + shootingDirection * particleAmplitudeSpeed_;
            emitParam.accelerationAmplitud_ = right * particleAmplitudeSpeed_ + up * particleAmplitudeSpeed_ + shootingDirection * particleAmplitudeSpeed_;
            particleEmitter_->SetEmitParamater(emitParam);
            break;
        }
        }
       

        rateTimer_ = rateOfFire_;

        
    }
    else return false;
    
    //TODO:弾数管理


    return true;
}

void Gun::UpdateFlashEffect()
{
    //エフェクトが非表示なら処理しない
    if (!muzzleFlashComponent_->GetVisibilty() && !beamMuzzleFlashComponent_->GetVisibilty())return;

    //マズル位置とアクター座標からエフェクトが出るべき座標のオフセット値を算出
    const Vector3 pos = transform_->GetPosition();
    const Vector3 offset = muzzlePos_ - pos;

    switch (bulletType_)
    {
    case Gun::BulletType::Bullet:
        muzzleFlashComponent_->SetOffsetPos(offset);
        //エフェクト寿命計算
        if (flashLifespan_ > Max_Flash_Lifespan)
        {
            muzzleFlashComponent_->SetVisibility(false);
        }
        break;
    case Gun::BulletType::Beam:
        beamMuzzleFlashComponent_->SetOffsetPos(offset);
        //エフェクト寿命計算
        if (flashLifespan_ > Max_Flash_Lifespan)
        {
            beamMuzzleFlashComponent_->SetVisibility(false);
        }
        break;
    }

    
}

void Gun::UpdateFlashParticleEffect()
{
    //フラグ判定
    if (!enableMuzzleFlashParticleEffect_)return;

    if (bulletType_ != BulletType::Beam)return;

    if (flashLifespan_ < flashParticleLifespan_)
    {
        //マズルフラッシュエフェクト再生
        particleEmitter_->SetEmitPositionNotUseTransform(muzzlePos_);
        particleEmitter_->EmitParticle();
    }
}
