#pragma once
#include "Component.h"
#include "MathHelper.h"

//�L�����N�^�[�̊��N���X
//CharacterManager����Update()���Ăт���
namespace AbyssEngine
{
    class SkeletalMesh;
    class AnimBlendSpace1D;

    class Character : public AbyssEngine::Component
    {
    public:
        virtual void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);
        virtual void Update() {}
        bool DrawImGui()override;

        //�����A�^�b�`�s��
        bool CanMultiple()override { return false; }

    public:
        const bool& GetIsActive() const { return isActive_; }
        void SetIsActive(const bool& active) { isActive_ = active; }


    protected:
        //�w������ɉ�]
        void TurnY(Vector3 dir,bool smooth = true/*�Ȃ߂炩�ɉ�]���邩*/);

    public:
        constexpr static float Gravity = 1.0f;

    protected:
        bool isActive_ = true;

        Vector3 velocity_;//���x
        float acceleration_ = 1.0f;//������
        float deceleration_ = 5.0f;//������

        float Max_Speed = 20.0f;//�ő呬�x

        float baseRotSpeed_ = 300.0f;//��]���x
        float Max_Rot_Speed = 800.0f;//�ő��]���x
        float Min_Rot_Speed = 300.0f;//�Œ��]���x

        std::shared_ptr<AbyssEngine::SkeletalMesh> model_;

        AnimBlendSpace1D* moveAnimation_;
    };
}

