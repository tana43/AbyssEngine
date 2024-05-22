#pragma once
#include "Renderer.h"

namespace AbyssEngine
{
    class GltfStaticMesh;

    class StaticMesh : public Renderer
    {
    public:
        StaticMesh() {}
        ~StaticMesh() = default;

        void Initialize(const std::shared_ptr<Actor>& actor)override;
        bool DrawImGui()override;

    private:
        void SetActive(const bool value)override;

        void Render()override;
        void RenderShadow()override;

    private:
        std::unique_ptr<GltfStaticMesh> model_;
    };
}

