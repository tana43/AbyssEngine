#pragma once
#include "Renderer.h"
#include "GltfStaticMesh.h"
#include "BoundingBox.h"

namespace AbyssEngine
{
    class StaticMesh : public Renderer
    {
    public:
        StaticMesh() {}
        ~StaticMesh() = default;

        void Initialize(const std::shared_ptr<Actor>& actor)override;
        bool DrawImGui()override;
        void RecalculateFrame()override;//çsóÒçXêVìô

    private:
        void SetActive(const bool value)override;

        void Render()override;
        void RenderShadow()override;
        bool FrustumCulling(const DirectX::BoundingFrustum& frustum);

    private:
        std::shared_ptr<GltfStaticMesh> model_;

        Vector3 minValue_;
        Vector3 maxValue_;
        DirectX::BoundingBox boundingBox_;//AABB

    };
}

