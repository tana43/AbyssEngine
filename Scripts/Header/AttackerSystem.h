#pragma once
#include "ScriptComponent.h"

#include <unordered_map>

namespace AbyssEngine
{
    class AttackerSystem : public ScriptComponent
    {
    public:
        //攻撃
        struct AttackData
        {
            float power_ = 0;//攻撃力
            float duration_ = 1.0f;//持続時間
            float staggerValue_ = 0;//スタッグ値（ひるみ値のようなもの）
            int maxHits = 1;//攻撃が連続ヒットできる回数　最低でも１は必要

            float hitStop_ = 0;//攻撃が当たった際にヒットストップする時間

            //判定を出現させるコライダー
            std::vector<std::weak_ptr<AttackCollider>> attackColliderList_;
        };

    public:
        AttackerSystem() {}
        ~AttackerSystem() {}

        void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;

        void Update()override;

        //何かに当たっている
        virtual void OnCollision(const std::shared_ptr<Collider>& collision, Collision::IntersectionResult result)override;

        void RegistAttackData(std::string atkName,const AttackData atkData);

    private:

        //全ての攻撃情報を管理しているマップ
        std::unordered_map<std::string,AttackData> attackDataMap_;

        //現在の攻撃
        AttackData currentAttack_;

        //攻撃が一度当たったか
        bool hit_;
        //現在の攻撃が何回ヒットしたか
        int hitCount_;

        //持続時間を計測するためのタイマー
        float durationTimer_ = 0;

        //アタックシステムを適用させるコライダー
        std::vector<std::weak_ptr<AttackCollider>> attackColliderList_;
    };
}
