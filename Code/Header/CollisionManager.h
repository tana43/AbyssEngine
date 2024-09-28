#pragma once

#include <memory>
#include <vector>

#include "Collider.h"

namespace AbyssEngine
{
    class MeshCollider;

    class AttackCollider;
    class HitCollider;

    class SphereCollider;


    class CollisionManager
    {
    public:
        CollisionManager() {}
        ~CollisionManager() {}

        void Initialize();

        void RayCast();

        void Finalieze();
        void Clear();

        void Update();

        void UpdateWorldMatrix();

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

        //攻撃判定コライダーを登録
        void RegisterAttackCollider(const std::shared_ptr<AttackCollider>& collider);

        //喰らい判定用コライダーを登録
        void RegisterHitCollider(const std::shared_ptr<HitCollider>& collider);

        //攻撃判定コライダーを削除
        void DeleteAttackCollider(const std::shared_ptr<SphereCollider>& collider);

        //喰らい判定用コライダーを削除
        void DeleteHitCollider(const std::shared_ptr<SphereCollider>& collider);

        //コライダーがヒットした際に呼ばれる関数
        void OnCollision(const std::shared_ptr<Collider>& myCollider,const std::shared_ptr<Collider>& collider,const Collision::IntersectionResult& result);

        const std::vector<std::weak_ptr<AttackCollider>>& GetAttackColliderList() const { return attackColliderList_; }
        const std::vector<std::weak_ptr<HitCollider>>& GetHitColliderList() const { return hitColliderList_; }

    private:

        std::vector<std::weak_ptr<MeshCollider>> meshColliderList_;

        //押し出し判定コライダー
        std::vector<std::weak_ptr<Collider>> terrainColliderList_;

        //攻撃判定コライダー
        std::vector<std::weak_ptr<AttackCollider>> attackColliderList_;

        //喰らい判定コライダー
        std::vector<std::weak_ptr<HitCollider>> hitColliderList_;
    };
}
