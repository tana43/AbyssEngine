#pragma once
#include "Renderer.h"

namespace AbyssEngine
{
    class FbxMeshData;
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
        std::unique_ptr<FbxMeshData> model_;

        Vector4 color_ = {1,1,1,1};
    };

    class GltfModel;

    class GltfSkeltalMesh : public Renderer
    {
        GltfSkeltalMesh() {}
        ~GltfSkeltalMesh() = default;

        void Initialize(const std::shared_ptr<Actor>& actor)override;
        void Render()override;

    private:
        void SetActive(const bool value)override;

    private:

        std::unique_ptr<SkeltalMesh> model_;
    };
}

