#pragma once
#include "Renderer.h"
#include "GeometricSubstance.h"

namespace AbyssEngine
{
    class GltfSkeletalMesh;
    class Actor;

    class SkeletalMesh : public Renderer
    {
    public:
        SkeletalMesh() {}
        ~SkeletalMesh() = default;

        void Initialize(const std::shared_ptr<Actor>& actor)override;
        void Render()override;

        //�A�j���[�V�����A�Z�b�g�̒ǉ�
        void AppendAnimation(const std::string& filename);
        void AppendAnimations(const std::vector<std::string>& filenames);

        void RecalculateFrame()override;

    private:
        void SetActive(const bool value)override;

    private:

        //�Ƃ肠�����`�悳���Ă݂����̂Ń��j�[�N�|�C���^
        //std::unique_ptr<FbxMeshData> model_;
        std::unique_ptr<GltfSkeletalMesh> model_;

        Vector4 color_ = {1,1,1,1};

        size_t animationClip_ = 0;
        float timeStamp_ = 0.0f;//���v�o�ߎ���

        float animationSpeed_ = 1.0f;
        std::vector<GeometricSubstance::Node> animatedNodes_;
    };

    /*class GltfModel;

    class GltfSkeltalMesh : public Renderer
    {
    public:
        GltfSkeltalMesh() {}
        ~GltfSkeltalMesh() = default;

        void Initialize(const std::shared_ptr<Actor>& actor)override;
        void Render()override;

    private:
        void SetActive(const bool value)override;

    private:

        std::unique_ptr<GltfModel> model_;
    };*/
}

