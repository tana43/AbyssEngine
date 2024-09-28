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

        //�U������R���C�_�[��o�^
        void RegisterAttackCollider(const std::shared_ptr<AttackCollider>& collider);

        //��炢����p�R���C�_�[��o�^
        void RegisterHitCollider(const std::shared_ptr<HitCollider>& collider);

        //�U������R���C�_�[���폜
        void DeleteAttackCollider(const std::shared_ptr<SphereCollider>& collider);

        //��炢����p�R���C�_�[���폜
        void DeleteHitCollider(const std::shared_ptr<SphereCollider>& collider);

        //�R���C�_�[���q�b�g�����ۂɌĂ΂��֐�
        void OnCollision(const std::shared_ptr<Collider>& myCollider,const std::shared_ptr<Collider>& collider,const Collision::IntersectionResult& result);

        const std::vector<std::weak_ptr<AttackCollider>>& GetAttackColliderList() const { return attackColliderList_; }
        const std::vector<std::weak_ptr<HitCollider>>& GetHitColliderList() const { return hitColliderList_; }

    private:

        std::vector<std::weak_ptr<MeshCollider>> meshColliderList_;

        //�����o������R���C�_�[
        std::vector<std::weak_ptr<Collider>> terrainColliderList_;

        //�U������R���C�_�[
        std::vector<std::weak_ptr<AttackCollider>> attackColliderList_;

        //��炢����R���C�_�[
        std::vector<std::weak_ptr<HitCollider>> hitColliderList_;
    };
}
