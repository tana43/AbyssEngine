#pragma once
#include "Renderer.h"
#include "Animator.h"
#include "GltfSkeletalMesh.h"

namespace AbyssEngine
{
    class Actor;
    class Animator;
    class StaticMesh;

    class SkeletalMesh : public Renderer
    {
    public:
        SkeletalMesh() {}
        ~SkeletalMesh() = default;

        void Initialize(const std::shared_ptr<Actor>& actor)override;
        

        //�A�j���[�V�����A�Z�b�g�̒ǉ�
        //void AppendAnimation(const std::string& filename);
        //void AppendAnimations(const std::vector<std::string>& filenames);

        //�A�j���[�V�����̍Čv�Z
        void RecalculateFrame()override;

        bool DrawImGui()override;

        //�A�j���[�V�����̍Đ�
        void PlayAnimation(int animIndex,bool loop = true);

        //�A�j���[�V�����̍Đ����x�ݒ�
        void SetAnimationSpeed(const float& speed) { animationSpeed_ = speed; }

        GltfSkeletalMesh* GetModel() { return model_.get(); }

        std::shared_ptr<Animator>& GetAnimator() { return animator_; }

        //�\�P�b�g�ɃA�^�b�`����@����Ȃǂ��������邱�Ƃ��o����
        void SocketAttach(const std::shared_ptr<StaticMesh>& attachModel, const char* socketName);

        DirectX::XMFLOAT4X4& FindSocket(const char* socketName);

    private:
        void SetActive(const bool value)override;

        void Render()override;
        void RenderShadow()override;

    private:

        //�Ƃ肠�����`�悳���Ă݂����̂Ń��j�[�N�|�C���^
        //std::unique_ptr<FbxMeshData> model_;
        std::shared_ptr<GltfSkeletalMesh> model_;

        //animator�R���|�[�l���g
        std::shared_ptr<Animator> animator_;

        Vector4 color_ = {1,1,1,1};

        size_t animationClip_ = 0;//�Đ����̃A�j���[�V�����ԍ�
        float timeStamp_ = 0.0f;//���v�o�ߎ���

        float animationSpeed_ = 1.0f;//�A�j���[�V�����Đ����x

        bool animationLoop_ = true;

        float blendWeight_ = 0.0f;//���̃��[�V�����u�����h�̏d��
    };
}

