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
        /// �n�`����(�����o��)
        /// </summary>
        void TerrainDetection();

        /// <summary>
        /// �U������
        /// </summary>
        void AttackDetection();

        /// <summary>
        /// ��炢����
        /// </summary>
        void HitDetection();

    private:
        std::vector<std::weak_ptr<Collider>> colliderList_;
        std::vector<std::weak_ptr<MeshCollider>> meshColliderList_;
        std::vector<std::weak_ptr<AttackCollider>> attackColliderList_;
        std::vector<std::weak_ptr<HitCollider>> hitColliderList_;
    };
}
