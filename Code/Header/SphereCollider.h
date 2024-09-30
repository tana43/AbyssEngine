#pragma once
#include "Collider.h"

namespace AbyssEngine
{
    class SkeletalMesh;

    class SphereCollider : public Collider
    {
    public:
        SphereCollider() {}
        ~SphereCollider() {}

        void Initialize(const std::shared_ptr<Actor>& actor)override;

        //���[���h�s��X�V
        void UpdateWorldMatrix()override;

        void DrawDebug()override;

        void DrawImGui()override;

        //���Ƃ̌�������
        bool IntersectVsSphere(const std::shared_ptr<SphereCollider>& collider, Collision::IntersectionResult* result);

        //���f���ɃA�^�b�`����
        void AttachModel(const std::shared_ptr<SkeletalMesh>& model, std::string socketName);



    public:
        const float& GetRadius() const { return radius_; }
        void SetRadius(const float& radius) { radius_ = radius; }

        void SetDebugColor(const Vector4& color) { debugColor_ = color; }

        const std::string& GetSocketName() const { return socketName_; }

    protected:
        //���a
        float radius_;

        //�f�o�b�O���ɕ\������鋅�̂̃J���[
        Vector4 debugColor_ = { 1,1,1,1 };

        //�A�^�b�`����ꍇ
        std::weak_ptr<SkeletalMesh> attachModel_;
        std::string socketName_;
    };
}

