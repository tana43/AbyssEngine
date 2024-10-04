#include "AttackerSystem.h"
#include "Engine.h"

using namespace AbyssEngine;

void AttackerSystem::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    //初期化
    ScriptComponent::Initialize(actor);
}

void AttackerSystem::Update()
{
    //ヒットフラグリセット
    hit_ = false;

    //タイマー更新
    durationTimer_ += Time::deltaTime_;
}

void AbyssEngine::AttackerSystem::OnCollision(const std::shared_ptr<Collider>& collision, Collision::IntersectionResult result)
{
    //持続時間を越えていないか
    if (durationTimer_ > currentAttack_.duration_)return;
}

void AbyssEngine::AttackerSystem::RegistAttackData(std::string atkName, const AttackData atkData)
{
    attackDataMap_.emplace(atkName, atkData);
}



