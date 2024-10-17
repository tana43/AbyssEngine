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
    durationTimer_ += actor_->GetDeltaTime();

    //攻撃が有効かを判定
    AttackEnabledUpdate();
    
}

void AbyssEngine::AttackerSystem::OnCollision(const std::shared_ptr<Collider>& collision, Collision::IntersectionResult result)
{
    //一度攻撃がヒットしたフレームか
    if (!attackEnabled_)return;

    //ダメージを与える
    const std::shared_ptr<Character>& target = collision->GetActor()->GetParent().lock()->GetComponent<Character>();
    if (target)
    {
        ApplyDamage(target);
    }

    //攻撃が有効かを更新
    AttackEnabledUpdate();
}

void AbyssEngine::AttackerSystem::RegistAttackData(std::string atkName, const AttackData atkData)
{
    attackDataMap_.emplace(atkName, atkData);
}

void AbyssEngine::AttackerSystem::RegistCollider(const std::shared_ptr<AttackCollider>& collider)
{
    attackColliderList_.emplace_back(collider);
    collider->SetEnable(false);
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
    currentAttack_.knockback_           = atkData->second.knockback_;
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
            atk->SetEnable(false);
            for (const auto& c : currentAttack_.attackColliderList_)
            {
                if (const auto& active = c.lock())
                {
                    if (atk == active)
                    {
                        atk->SetEnable(true);
                        break;
                    }
                }
            }
        }
    }
}

void AbyssEngine::AttackerSystem::ApplyDamage(const std::shared_ptr<Character>& target)
{
    Character::DamageResult result;
    Character::AttackParameter param;
    param.power_ = currentAttack_.power_;
    param.knockback_ = currentAttack_.knockback_;

    //攻撃方向を算出
    const Vector3 vec = transform_->GetPosition() - target->GetTransform()->GetPosition();
    vec.Normalize(param.vector_);

    if (target->ApplyDamage(param, &result))
    {
        //攻撃がヒットした
        hit_ = true;
        hitCount_++;

        //ヒットインターバルをリセット
        hitIntervalTimer_ = 0;
    }
}

void AbyssEngine::AttackerSystem::AttackEnabledUpdate()
{
    //更新前の値
    bool cachedEnabled = attackEnabled_;

    //攻撃が有効な状態か判定
    if (hit_ || /*一度攻撃がヒットしたフレームか*/
        durationTimer_ > currentAttack_.duration_ ||/*持続時間を越えていないか*/
        hitCount_ >= currentAttack_.maxHits_ || /*連続攻撃の最大ヒット数を越えていないか*/
        hitIntervalTimer_ < currentAttack_.hitInterval_)/*ヒット後のインターバル中か*/
    {
        attackEnabled_ = false;
    }
    else
    {
        attackEnabled_ = true;
    }

    //オンオフが切り替わったならコライダーも設定しなおす
    if (attackEnabled_ != cachedEnabled)
    {
        for (const auto& col : currentAttack_.attackColliderList_)
        {
            if (const auto& collider = col.lock())
            {
                collider->SetEnable(attackEnabled_);
            }
        }
    }
}



