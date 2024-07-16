#pragma once
#include "ScriptComponent.h"
#include "MathHelper.h"

//�L�����N�^�[�̊��N���X
//CharacterManager����Update()���Ăт���
namespace AbyssEngine
{
    class SkeletalMesh;
    class Animator;
    class AnimBlendSpace1D;

    class Character : public ScriptComponent
    {
    public:
        virtual void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);
        virtual void Update()override {}
        //virtual void DelayedUpdate() {}//�`�掞�̍s��X�V�Ȃǂ��I����Ă���

        bool DrawImGui()override;

        //�����A�^�b�`�s��
        bool CanMultiple()override { return false; }

        void Jump(const float& jumpPower);

    public:
        const bool& GetIsActive() const { return isActive_; }
        void SetIsActive(const bool& active) { isActive_ = active; }

        const std::shared_ptr<SkeletalMesh>& GetModel() { return model_; }
        const std::shared_ptr<Animator>& GetAnimator();

        const Vector3& GetVelocity() { return velocity_; }
        void SetVelocity(const Vector3& velo) { velocity_ = velo; }
        void SetVelocityX(const float& x) { velocity_.x = x; }
        void SetVelocityY(const float& y) { velocity_.y = y; }
        void SetVelocityZ(const float& z) { velocity_.z = z; }

        const float GetMaxHorizontalSpeed() const { return Max_Horizontal_Speed; }
        const float GetMaxVerticalSpeed()const { return Max_Vertical_Speed; }

    protected:
        //�w������ɉ�]
        void TurnY(Vector3 dir,bool smooth = true/*�Ȃ߂炩�ɉ�]���邩*/);

        virtual void Move();//�ړ�����

        virtual void UpdateVelocity();//���͍X�V
        virtual void UpdateMove();
        virtual void UpdateHorizontalMove();//�n�ʂɑ΂��Đ��������̈ړ������ɂ��ʒu�X�V
        virtual void UpdateVerticalMove();//�n�ʂɑ΂��Đ��������̈ړ������ɂ��ʒu�X�V

        virtual void Landing();//���n
    public:
        constexpr static float Gravity = -9.8f;

    protected:
        float weight_ = 1.0f;//�d���i�d�͂̌v�Z�Ɏg���j

        bool isActive_ = true;
        bool onGround_ = true;

        Vector3 moveVec_;//�ړ�����
        Vector3 velocity_;//���x
        float acceleration_ = 1.0f;//������
        float deceleration_ = 5.0f;//������

        float Max_Horizontal_Speed = 20.0f;//���������ɑ΂���ő呬�x
        float Max_Vertical_Speed = 20.0f;//�c�����ɑ΂���ő呬�x

        float baseRotSpeed_ = 300.0f;//��]���x
        float Max_Rot_Speed = 800.0f;//�ő��]���x
        float Min_Rot_Speed = 300.0f;//�Œ��]���x

        float airborneCoefficient_ = 0.3f;//�󒆂ɂ���ۂɒn������ړ��̎��R�������Ȃ����邽�߂̒l
        float airResistance_ = 0.2f;//��C��R

        std::shared_ptr<AbyssEngine::SkeletalMesh> model_;

        AnimBlendSpace1D* moveAnimation_;//����ړ�
    };
}

