#pragma once
#include "Renderer.h"
#include "GltfStaticMesh.h"

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

    private:
        std::unique_ptr<GltfStaticMesh> model_;
    };
}

