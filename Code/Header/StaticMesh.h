#pragma once
#include "Renderer.h"
#include "GltfStaticMesh.h"
#include "BoundingBox.h"
#include "MathHelper.h"
#include <memory>

namespace AbyssEngine
{
    class SkeletalMesh;

    class StaticMesh : public Renderer
    {
    public:
        StaticMesh() {}
        ~StaticMesh() = default;

        void Initialize(const std::shared_ptr<Actor>& actor)override;
        bool DrawImGui()override;
        void RecalculateFrame()override;//行列更新等

        struct Socket
        {
        public:
            std::shared_ptr<SkeletalMesh> attachedMesh_;
            Matrix socketTrasnform_;
            std::string attachedSocketName_;
            Vector3 location_ = {0,0,0};
            Vector3 rotation_ = {0,0,0};
            Vector3 scale_ = {1,1,1};
        };
        bool isAttached_ = false;

        [[nodiscard]]Socket& GetSocketData() { return socketData_; }

    private:
        void SetActive(const bool value)override;

        void Render()override;
        void RenderShadow()override;
        bool FrustumCulling(const DirectX::BoundingFrustum& frustum);
    private:
        //ワールド行列
        Matrix world_;

        std::shared_ptr<GltfStaticMesh> model_;

        Vector3 minValue_;
        Vector3 maxValue_;
        DirectX::BoundingBox boundingBox_;//AABB

        //ソケットアタッチ
        Socket socketData_;

        
    };
}

