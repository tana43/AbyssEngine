#pragma once
#include "ScriptComponent.h"

namespace AbyssEngine
{
    class Transform;
    class ComputeParticleEmitter;
}

class SpaceParticleEffect : public AbyssEngine::ScriptComponent
{
public:
    void Initialize(const std::shared_ptr<AbyssEngine::Actor> actor);
    void Update();

private:
    //何を中心にパーティクルを生成させるか（基本カメラ）
    //std::shared_ptr<AbyssEngine::Transform> attachTransform_;


    std::shared_ptr<AbyssEngine::ComputeParticleEmitter> particleEmitter_;

    //生成頻度
    float emitFrequency_ = 0.1f;
    float timer_ = 0.0f;
};

