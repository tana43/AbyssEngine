#pragma once
#include "Renderer.h"
#include "MeshData.h"

namespace AbyssEngine
{
    class SkeltalMesh : public Renderer
    {
    public:
        SkeltalMesh() {}
        ~SkeltalMesh() = default;

        void Initialize(const std::shared_ptr<Actor>& actor)override;
        void Render()override;

    private:
        //�Ƃ肠�����`�悳���Ă݂����̂Ń��j�[�N�|�C���^
        std::unique_ptr<MeshData> model_;

        Vector4 color_ = {1,1,1,1};
    };
}

