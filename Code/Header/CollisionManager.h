#pragma once

#include <memory>
#include <vector>

namespace AbyssEngine
{
    class Collider;
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
        /// ’nŒ`”»’è(‰Ÿ‚µo‚µ)
        /// </summary>
        void TerrainDetection();

        /// <summary>
        /// UŒ‚”»’è
        /// </summary>
        void AttackDetection();

        /// <summary>
        /// ‹ò‚ç‚¢”»’è
        /// </summary>
        void HitDetection();

    private:
        std::vector<std::weak_ptr<Collider>> colliderList_;
        std::vector<std::weak_ptr<MeshCollider>> meshColliderList_;
        std::vector<std::weak_ptr<AttackCollider>> attackColliderList_;
        std::vector<std::weak_ptr<HitCollider>> hitColliderList_;
    };
}
