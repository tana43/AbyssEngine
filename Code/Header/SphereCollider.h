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

        //ワールド行列更新
        void UpdateWorldMatrix()override;

        void DrawDebug()override;

        void DrawImGui()override;

        //球との交差判定
        bool IntersectVsSphere(const std::shared_ptr<SphereCollider>& collider, Collision::IntersectionResult* result);

        //モデルにアタッチする
        void AttachModel(const std::shared_ptr<SkeletalMesh>& model, std::string socketName);



    public:
        const float& GetRadius() const { return radius_; }
        void SetRadius(const float& radius) { radius_ = radius; }

        void SetDebugColor(const Vector4& color) { debugColor_ = color; }

        const std::string& GetSocketName() const { return socketName_; }

    protected:
        //半径
        float radius_;

        //デバッグ時に表示される球体のカラー
        Vector4 debugColor_ = { 1,1,1,1 };

        //アタッチする場合
        std::weak_ptr<SkeletalMesh> attachModel_;
        std::string socketName_;
    };
}

