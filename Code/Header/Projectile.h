#pragma once
#include "ScriptComponent.h"
#include "Stage.h"

namespace AbyssEngine
{
    //飛び道具コンポーネント
    class Projectile : public ScriptComponent
    {
    public:
        Projectile() {}
        ~Projectile() {}

        void Initialize(const std::shared_ptr<Actor> actor);
        void Update();

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

    private:
        //進行方向
        Vector3 direction_;

        //速度
        float speed_;

        //当たり判定用の半径
        float radius_ = 0.1f;
    };
}

