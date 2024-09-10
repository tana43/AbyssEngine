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

        void DrawImGui()override;

        //�A�j���[�V�����̍Đ�
        void PlayAnimation(int animIndex,bool loop = true);

        //�A�j���[�V�����̍Đ����x�ݒ�
        void SetAnimationSpeed(const float& speed) { animationSpeed_ = speed; }

        //�\�P�b�g�ɃA�^�b�`����@����Ȃǂ��������邱�Ƃ��o����
        void SocketAttach(const std::shared_ptr<StaticMesh>& attachModel, const char* socketName);

        const Matrix& FindSocket(const char* socketName);

    public:
        GltfSkeletalMesh* GetModel() { return model_.get(); }

        const std::shared_ptr<Animator>& GetAnimator() { return animator_; }

        void SetOffsetRotation(const Vector3& rot) { offsetRot_ = rot; }

    private:
        void SetActive(const bool value)override;

        void Render()override;
        void RenderShadow()override;
        bool FrustumCulling(const DirectX::BoundingFrustum& frustum)override;
        bool ShadowCulling(const DirectX::BoundingBox& box)override;

    private:
        //���[���h�s��
        Matrix worldMatrix_;

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

        //�o�E���f�B���O�{�b�N�X(�t���X�^���J�����O�p)
        Vector3 minValue_;
        Vector3 maxValue_;
        DirectX::BoundingBox boundingBox_;//AABB

        //�I�t�Z�b�g��]�l
        Vector3 offsetRot_ = {0,0,0};
    };
}

