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

        std::vector<std::weak_ptr<MeshCollider>> meshColliderList_;

        //�����o������R���C�_�[
        std::vector<std::weak_ptr<Collider>> terrainColliderList_;

        //�U������R���C�_�[
        std::vector<std::weak_ptr<Collider>> attackColliderList_;

        //��炢����R���C�_�[
        std::vector<std::weak_ptr<Collider>> hitColliderList_;
    };
}
