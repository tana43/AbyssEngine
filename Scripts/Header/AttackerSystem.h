#pragma once
#include "ScriptComponent.h"

#include <unordered_map>

namespace AbyssEngine
{
    class Character;
    class AttackCollider;

    //攻撃データ
    struct AttackData
    {
        float power_ = 0;//攻撃力
        float knockback_ = 0;//吹っ飛ばし力
        float duration_ = 1.0f;//持続時間
        float staggerValue_ = 0;//スタッグ値（ひるみ値のようなもの）

        int maxHits_ = 1;//攻撃が連続ヒットできる回数　最低でも１は必要
        float hitInterval_ = 0.2f;//攻撃が連続ヒットする際に何秒ごとに攻撃を

        float hitStop_ = 0;//攻撃が当たった際にヒットストップする時間

        //判定を出現させるコライダー
        std::vector<std::weak_ptr<AttackCollider>> attackColliderList_;

        //Game:ヒット音とかも入れるといいかも
    };

    class AttackerSystem : public ScriptComponent
    {
    public:
        AttackerSystem() {}
        ~AttackerSystem() {}

        void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;

        void Update()override;

        //何かに当たっている
        virtual void OnCollision(const std::shared_ptr<Collider>& collision, Collision::IntersectionResult result)override;

        //AttackDataを追加
        void RegistAttackData(std::string atkName,const AttackData atkData);

        //コライダーを追加
        void RegistCollider(const std::shared_ptr<AttackCollider>& collider);

        //指定した名前のデータを使って攻撃を開始する
        void Attack(std::string atkName);

    private:
        void ApplyDamage(const std::shared_ptr<Character>& target);

        //現在攻撃が有効な状態かの判定をする
        void AttackEnabledUpdate();

    private:

        //全ての攻撃情報を管理しているマップ
        std::unordered_map<std::string,AttackData> attackDataMap_;

        //現在の攻撃
        AttackData currentAttack_;

        //攻撃が一度当たったか
        bool hit_ = false;
        //現在の攻撃が何回ヒットしたか
        int hitCount_ = 0;
        //攻撃が連続ヒットしたときのヒット間隔を計測するタイマー
        int hitIntervalTimer_ = 0;

        //持続時間を計測するためのタイマー
        float durationTimer_ = 0;

        //アタックシステムを適用させるコライダー
        std::vector<std::weak_ptr<AttackCollider>> attackColliderList_;

        //現在攻撃が有効な状態か
        bool attackEnabled_ = false;
    };
}
