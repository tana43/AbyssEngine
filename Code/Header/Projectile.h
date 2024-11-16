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
        //進行方向ベクトルから回転行列を算出
        void SetDirection(const Vector3& dir);

        const std::shared_ptr<AttackCollider>& GetAtkCollider() const { return atkCollider_; }

        void SetRadius(const float& radius);

        void SetSpeed(const float& speed) { speed_ = speed; }

        void SetTargetTag(const unsigned int& actorTag) { targetTag_ = actorTag; }

        void SetHomingStrength(const float& strength) { homingStrength_ = strength; }

        void SetIsHoming(const bool& active) { isHoming_ = active; }

        void SetLifespan(const float& lifespan) { lifespan_ = lifespan; }

        void SetTargetTransfrom(const std::shared_ptr<Transform>& t) { targetTransform_ = t; }

        void SetTerrainHitPos(const Vector3& pos) 
        {
            //この関数が呼ばれるときは地形に当たるということになるから、フラグも同時に上げておく
            terrainHitPos_ = pos; 
            isTerrainHit_ = true;
        }

        //地形に当たった際に呼び出される
        virtual void HitTerrain() {}

    private:
        void LifeTimeUpdate();

        //直進移動更新
        void MoveUpdate();

        //ホーミング移動更新
        void HomingUpdate();

        //地形判定を越えていないか判定
        void IsTerrainHitUpdate();

        //targetPosにホーミングさせる
        void Homing(Vector3 targetPos);

    protected:
        //進行方向
        //Vector3 direction_;

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

        //ターゲットに一定距離近づいたらホーミングをさせないようにする
        bool homingExpired_ = false;

        //ホーミングを終了させる距離
        float homingExpiredLength_ = 30.0f;

        //ホーミングするターゲットのタグ
        unsigned int targetTag_ = 0;

        //ホーミングターゲット
        std::weak_ptr<Transform> targetTransform_;

        //ホーミング強度
        float homingStrength_ = 1.0f;

        //直進弾のときは、あらかじめ弾が地形に当たる位置を設定しておく
        Vector3 terrainHitPos_ = {};

        //そもそも地形に当たるのか
        bool isTerrainHit_ = false;
    };
}

