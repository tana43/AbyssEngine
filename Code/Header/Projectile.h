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

        const std::shared_ptr<SphereCollider>& GetCollider() const { return collider_; }

        void SetRadius(const float& radius);

    protected:
        //進行方向
        Vector3 direction_;

        //速度
        float speed_ = 50.0f;

        //当たり判定用の半径
        float radius_ = 0.1f;

        //寿命
        float flashLifespan_ = 10.0f;

        //球判定
        std::shared_ptr<SphereCollider> collider_;
    };
}

