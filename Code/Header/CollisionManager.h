#pragma once

#include <memory>
#include <vector>

#include "Collider.h"

namespace AbyssEngine
{
    class MeshCollider;

    class AttackCollider;
    class HitCollider;


    class CollisionManager
    {
    public:
        CollisionManager() {}
        ~CollisionManager() {}

        void Initialize();

        void RayCast();

        void Finalieze();
        void Clear();

        /// <summary>
        /// 地形判定(押し出し)
        /// </summary>
        void TerrainDetection();

        /// <summary>
        /// 攻撃判定
        /// </summary>
        void AttackDetection();

        /// <summary>
        /// 喰らい判定
        /// </summary>
        void HitDetection();

    private:

        std::vector<std::weak_ptr<MeshCollider>> meshColliderList_;

        //押し出し判定コライダー
        std::vector<std::weak_ptr<Collider>> terrainColliderList_;

        //攻撃判定コライダー
        std::vector<std::weak_ptr<Collider>> attackColliderList_;

        //喰らい判定コライダー
        std::vector<std::weak_ptr<Collider>> hitColliderList_;
    };
}
