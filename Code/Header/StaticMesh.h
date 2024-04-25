#pragma once
#include "Renderer.h"

namespace AbyssEngine
{
    class StaticMeshBatching;

    class StaticMesh : public Renderer
    {
    public:
        StaticMesh() {}
        ~StaticMesh() = default;

        void Initialize(const std::shared_ptr<Actor>& actor)override;
        void Render()override;

    private:
        void SetActive(const bool value)override;

    private:
        std::unique_ptr<StaticMeshBatching> model_;
    };
}

