#pragma once
#include "Renderer.h"

namespace AbyssEngine
{
    class MeshData;
    class Actor;

    class SkeltalMesh : public Renderer
    {
    public:
        SkeltalMesh() {}
        ~SkeltalMesh() = default;

        void Initialize(const std::shared_ptr<Actor>& actor)override;
        void Render()override;

    private:
        void SetActive(const bool value)override;

    private:

        //とりあえず描画させてみたいのでユニークポインタ
        std::unique_ptr<MeshData> model_;

        Vector4 color_ = {1,1,1,1};
    };
}

