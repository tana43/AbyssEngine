#include "SpaceParticleEffect.h"
#include "Actor.h"
#include "ComputeParticleEmitter.h"
#include "Engine.h"
#include "Camera.h"

using namespace AbyssEngine;

void SpaceParticleEffect::Initialize(const std::shared_ptr<AbyssEngine::Actor> actor)
{
    ScriptComponent::Initialize(actor);

    //パーティクルコンポーネント追加　初期設定
    particleEmitter_ = actor_->AddComponent<ComputeParticleEmitter>();
    particleEmitter_->SetEmitParamater("SpaceParticles");
}

void SpaceParticleEffect::Update()
{
    //パーティクル生成
    if (timer_ > emitFrequency_)
    {
        //カメラの座標まで移動処理
        Vector3 cameraPos = Camera::GetMainCamera()->GetTransform()->GetPosition();
        transform_->SetPosition(cameraPos);

        particleEmitter_->EmitParticle();

        timer_ = 0.0f;
    }

    //タイマー更新
    timer_ += actor_->GetDeltaTime();
}
