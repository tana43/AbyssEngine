#pragma once
#include "Component.h"
#include "CollisionHelper.h"
#include "MathHelper.h"

#include <string>

namespace AbyssEngine
{
    class Collider;
    class SphereCollider;
    class SkeletalMesh;

    //�Ǝ��̃R���|�[�l���g���J������ۂɌp�����Ƃ��Ďg�p����R���|�[�l���g
    //Update�����������Ă���
    class ScriptComponent : public Component
    {
    public:
        ScriptComponent() {}
        ~ScriptComponent() {}

        void Initialize(const std::shared_ptr<Actor>& actor)override;

        //�����ɓ������Ă���ꍇ�����ŌĂ΂��
        virtual void OnCollision(const std::shared_ptr<Collider>& collision, Collision::IntersectionResult result) {};

        virtual void UpdateBegin() {}
        virtual void UpdateBefore() {}
        virtual void Update() {}
        virtual void UpdateAfter() {}
        virtual void UpdateEnd() {}

        //�R���C�_�[��t����
        void AddTerrainCollider();  //�����o������p�R���C�_�[
        std::shared_ptr<SphereCollider> AddAttackCollider(Vector3 localPos, float radius, std::string name = "AtkCollider", const std::shared_ptr<SkeletalMesh>& attachModel = NULL, std::string socketName = "");   //�U������p�R���C�_�[
        std::shared_ptr<SphereCollider> AddHitCollider(Vector3 localPos, float radius, std::string name = "HitCollider",const std::shared_ptr<SkeletalMesh>& attachModel = NULL, std::string socketName = "");      //��炢����p�R���C�_�[

    public:
        const bool& GetActive() const { return active_; }
        void SetActive(const bool& active) { active_ = active; }

    private:
        bool active_ = true;
    };
}

