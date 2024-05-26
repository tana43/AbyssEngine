#pragma once
#include "Renderer.h"
#include "Animation.h"
#include "GltfSkeletalMesh.h"

namespace AbyssEngine
{
    class Actor;

    class SkeletalMesh : public Renderer
    {
    public:
        SkeletalMesh() {}
        ~SkeletalMesh() = default;

        void Initialize(const std::shared_ptr<Actor>& actor)override;
        

        //�A�j���[�V�����A�Z�b�g�̒ǉ�
        void AppendAnimation(const std::string& filename);
        void AppendAnimations(const std::vector<std::string>& filenames);

        //�A�j���[�V�����̍Čv�Z
        void RecalculateFrame()override;

        bool DrawImGui()override;

        //�A�j���[�V�����̍Đ�
        void PlayAnimation(int animIndex,bool loop = true);

        //�A�j���[�V�����̍Đ����x�ݒ�
        void SetAnimationSpeed(const float& speed) { animationSpeed_ = speed; }

        GltfSkeletalMesh* GetModel() { return model_.get(); }

    private:
        void SetActive(const bool value)override;

        void Render()override;
        void RenderShadow()override;
    private:

        //�Ƃ肠�����`�悳���Ă݂����̂Ń��j�[�N�|�C���^
        //std::unique_ptr<FbxMeshData> model_;
        std::unique_ptr<GltfSkeletalMesh> model_;

        Vector4 color_ = {1,1,1,1};

        size_t animationClip_ = 0;//�Đ����̃A�j���[�V�����ԍ�
        float timeStamp_ = 0.0f;//���v�o�ߎ���

        float animationSpeed_ = 1.0f;//�A�j���[�V�����Đ����x
        std::vector<GeometricSubstance::Node> animatedNodes_;

        bool animationLoop_ = true;

        float blendWeight_ = 0.0f;//���̃��[�V�����u�����h�̏d��
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

