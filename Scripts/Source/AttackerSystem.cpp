#include "AttackerSystem.h"
#include "Engine.h"
#include "Character.h"
#include "GameCollider.h"
#include "Actor.h"

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
    //一度攻撃がヒットしたフレームか
    if (hit_)return;

    //持続時間を越えていないか
    if (durationTimer_ > currentAttack_.duration_)return;

    //連続攻撃
    if (hitCount_ >= currentAttack_.maxHits_)return;

    //ヒット後のインターバル中か
    if (hitIntervalTimer_ < currentAttack_.hitInterval_)return;


    //ダメージを与える
    const std::shared_ptr<Character>& target = collision->GetActor()->GetComponent<Character>();
    if (target)
    {
        ApplyDamage(target);
    }
}

void AbyssEngine::AttackerSystem::RegistAttackData(std::string atkName, const AttackData atkData)
{
    attackDataMap_.emplace(atkName, atkData);
}

void AbyssEngine::AttackerSystem::RegistCollider(const std::shared_ptr<AttackCollider>& collider)
{
    attackColliderList_.emplace_back(collider);
}

void AbyssEngine::AttackerSystem::Attack(std::string atkName)
{
    auto atkData = attackDataMap_.find(atkName);
    if (atkData == attackDataMap_.end())
    {
        _ASSERT_EXPR(false, L"指定の名前のAttackDataが存在しません");
        return;
    }
    
    //攻撃の情報を設定
    currentAttack_.power_               = atkData->second.power_;
    currentAttack_.duration_            = atkData->second.duration_;
    currentAttack_.staggerValue_        = atkData->second.staggerValue_;
    currentAttack_.maxHits_             = atkData->second.maxHits_;
    currentAttack_.hitInterval_         = atkData->second.hitInterval_;
    currentAttack_.hitStop_             = atkData->second.hitStop_;
    currentAttack_.attackColliderList_  = atkData->second.attackColliderList_;

    //各値をリセット
    hitCount_ = 0;
    hitIntervalTimer_ = 1000.0f;//最初に攻撃が当たらないことがないように大きめの値をセット
    durationTimer_ = 0;

    //指定コライダーのみアクティブに
    for (const auto& p : attackColliderList_)
    {
        if (const auto& atk = p.lock())
        {
            for (const auto& c : currentAttack_.attackColliderList_)
            {
                if (const auto& active = c.lock())
                {
                    if (atk == active)
                    {
                        atk->SetEnable(true);
                    }
                    else
                    {
                        atk->SetEnable(false);
                    }
                }
            }
        }
    }
}

void AbyssEngine::AttackerSystem::ApplyDamage(const std::shared_ptr<Character>& target)
{
    Character::DamageResult result;
    if (target->ApplyDamage(currentAttack_.power_, result))
    {
        //攻撃がヒットした
        hit_ = true;
        hitCount_++;

        //ヒットインターバルをリセット
        hitIntervalTimer_ = 0;
    }
}



