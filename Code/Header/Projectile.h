#pragma once
#include "ScriptComponent.h"
#include "Stage.h"

namespace AbyssEngine
{
    class SphereCollider;

    //飛び道具コンポーネント
    class Projectile : public ScriptComponent
    {
    public:
        Projectile() {}
        ~Projectile() {}

        void Initialize(const std::shared_ptr<Actor>& actor);
        void Update()override;

        /// <summary>
        /// レイキャスト
        /// 生成と同時に地形とレイキャストをしておき、弾が敵に当たらなかったとき、
        /// どこの地形にヒットするかをあらかじめ算出しておく
        /// </summary>
        /// <param name="stage">判定対象になるステージコンポーネント</param>
        /// <param name="distance">レイの長さを入れ、処理後レイのヒット位置が代入されて返ってくる</param>
        /// <returns>レイは当たったか</returns>
        bool RayCast(const std::shared_ptr<Stage>& stage,
            float& distance,
            Vector3& hitPosition);

    public:
        void SetDirection(const Vector3& dir) { direction_ = dir; }

        const std::shared_ptr<AttackCollider>& GetAtkCollider() const { return atkCollider_; }

        void SetRadius(const float& radius);

        void SetSpeed(const float& speed) { speed_ = speed; }

        void SetTargetTag(const unsigned int& actorTag) { targetTag_ = actorTag; }

        void SetHomingStrength(const float& strength) { homingStrength_ = strength; }

        void SetIsHoming(const bool& active) { isHoming_ = active; }

        void SetLifespan(const float& lifespan) { lifespan_ = lifespan; }

        void SetTargetTransfrom(const std::shared_ptr<Transform>& t) { targetTransform_ = t; }

    private:
        void LifeTimeUpdate();

        //直進移動更新
        void MoveUpdate();

        //ホーミング移動更新
        void HomingUpdate();

    protected:
        //進行方向
        Vector3 direction_;

        //速度
        float speed_ = 50.0f;

        //当たり判定用の半径
        float radius_ = 0.1f;

        //寿命
        float lifespan_ = 2.0f;

        //球判定
        std::shared_ptr<AttackCollider> atkCollider_;

        //敵へホーミングするか
        bool isHoming_ = false;

        //ホーミングするターゲットのタグ
        unsigned int targetTag_ = 0;

        //ホーミングターゲット
        std::weak_ptr<Transform> targetTransform_;

        //ホーミング強度
        float homingStrength_ = 1.0f;
    };
}

