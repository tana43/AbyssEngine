#pragma once
#include "ScriptComponent.h"
#include "MathHelper.h"

#include <string>

//�L�����N�^�[�̊��N���X
//CharacterManager����Update()���Ăт���
namespace AbyssEngine
{
    class SphereCollider;

    class Character : public ScriptComponent
    {
    public:
        using Tag = unsigned int;

        //�^�O�̐ݒ�
        //�G�����̔��ʂ��s��
        static constexpr Tag Tag_Default    = 0x01;
        static constexpr Tag Tag_Player     = 0x01 << 1;
        static constexpr Tag Tag_Enemy      = 0x01 << 2;

        enum class DamageResult
        {
            Failed,     //���s
            Success,    //����
            FinalBlow   //�Ƃǂ�
        };

        struct AttackParameter
        {
            float power_ = 1.0f;
            float knockback_ = 1.0f;
            Vector3 vector_ = {};
        };

        Tag GetTag() const { return tag_; }
        void ReplaceTag(Tag t) { tag_ = t; }
        void AddTag(Tag t)
        {
            tag_ |= t;
        }

    public:
        virtual void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
        virtual void Update()override;
        //virtual void DelayedUpdate() {}//�`�掞�̍s��X�V�Ȃǂ��I����Ă���

        void DrawImGui()override;

        //�����A�^�b�`�s��
        bool CanMultiple()override { return false; }

        bool Jump(const float& jumpPower);

        //�w������ɉ�]
        void TurnY(Vector3 dir,bool smooth = true/*�Ȃ߂炩�ɉ�]���邩*/);
        void TurnY(Vector3 dir, const float& speed, bool smooth = true/*�Ȃ߂炩�ɉ�]���邩*/);

        //�_���[�W��^���� �U�����ʂ����Ȃ�true
        virtual bool ApplyDamage(const AttackParameter& param,DamageResult* damageResult = nullptr);

        //���S
        void Die();
        
        //�q�b�g�X�g�b�v�@�������Ԃƃt�F�[�h�J�n����
        void HitStop(float duration,float blendOutTime);

        //������΂� ���̂܂ܐ�����΂��x�N�g��������
        void AddImpulse(Vector3 impulse);

    public:
        const bool& GetIsActive() const { return active_; }
        void SetIsActive(const bool& active) { active_ = active; }

        const Vector3& GetVelocity() { return velocity_; }
        void SetVelocity(const Vector3& velo) { velocity_ = velo; }
        void SetVelocityX(const float& x) { velocity_.x = x; }
        void SetVelocityY(const float& y) { velocity_.y = y; }
        void SetVelocityZ(const float& z) { velocity_.z = z; }

        const float& GetMaxHorizontalSpeed() const { return Max_Horizontal_Speed; }
        void SetMaxHorizontalSpeed(const float& speed) { Max_Horizontal_Speed = speed; }
        const float& GetMaxVerticalSpeed()const { return Max_Vertical_Speed; }
        void SetMaxVerticalSpeed(const float& speed) { Max_Vertical_Speed = speed; }

        void SetEnableAutoTurn(const bool& flag) { enableAutoTurn_ = flag; }

        const bool& GetOnGround() const { return onGround_; }
        void SetOnGround(const bool& onGround) { onGround_ = onGround; }

        const float& GetAcceleration() const { return acceleration_; }
        void SetAcceleration(const float& accel) { acceleration_ = accel; }
        const float& GetDeceleration()const { return deceleration_; }
        void SetDeceleration(const float& decel) { deceleration_ = decel; }

        const AbyssEngine::Vector3& GetMoveVec() const { return moveVec_; }
        void SetMoveVec(const AbyssEngine::Vector3& vec) { moveVec_ = vec; }

        Vector3 GetCenterPos();

        const AbyssEngine::Vector3& GetExternalFactorsMove() const { return externalFactorsMove_; }
        //void SetExternalFactorsMove(const AbyssEngine::Vector3& move) { externalFactorsMove_ = move; } //�Z�b�g�͋֎~�I�I�I
        void AddExternalFactorsMove(const AbyssEngine::Vector3& move) { externalFactorsMove_ = externalFactorsMove_ + move; }

    protected:

        virtual void UpdateVelocity();//���͍X�V
        virtual void UpdateMove();

        //���C�L���X�g/�X�t�B�A�L���X�g�����ɑΉ�
        virtual void UpdateHorizontalMove();//�n�ʂɑ΂��Đ��������̈ړ������ɂ��ʒu�X�V
        virtual void UpdateVerticalMove();//�n�ʂɑ΂��Đ��������̈ړ������ɂ��ʒu�X�V

        virtual void Landing();//���n

        float Gravity = -9.8f;

    private:
        void UpdateImpactMove();//�m�b�N�o�b�N�Ȃǂɂ���ē�������鏈���̍X�V

        void UpdateHitStop();//�q�b�g�X�g�b�v�X�V�����@


    protected:
        Tag tag_ = Tag_Default;

        float weight_ = 1.0f;//�d���i�d�́A������΂��̌v�Z�Ɏg���j

        bool active_ = true;
        bool onGround_ = true;
        bool enableGravity_ = true;
        bool sphereCast_ = true;//�n�`����ɃX�t�B�A�L���X�g���g�p���邩�ifalse�̏ꍇ���C�L���X�g���g�p�j

        float terrainRadius_ = 0.18f;//�n�`����p�̋��̔��a
        float terrainStepOffset_ = 0.14f;//�X�t�B�A�L���X�g�Ŏn�_�ɂ���ʒu�𑫌�����ǂ̒��x�グ�邩
        float terrainCenterOffset_ = 0.3f;//���S
        float terrainSkinWidth_ = 0.05f;//�L���X�g�ʉ��Z�l

        float slopeLimit = 60.0f;//�X���[�v�p�x����

        Vector3 moveVec_;//�ړ�����
        Vector3 velocity_;//���x
        float acceleration_ = 1.0f;//������
        float deceleration_ = 5.0f;//������
        float speedingDecel_ = 10.0f;//�������x�𒴉߂��Ă��܂��Ă���Ƃ��Ɍ����������鑬�x

        float Max_Horizontal_Speed = 20.0f;//���������ɑ΂���ő呬�x
        float Max_Vertical_Speed = 20.0f;//�c�����ɑ΂���ő呬�x

        float baseRotSpeed_ = 300.0f;//��]���x
        float Max_Rot_Speed = 800.0f;//�ő��]���x
        float Min_Rot_Speed = 300.0f;//�Œ��]���x

        float airborneCoefficient_ = 0.3f;//�󒆂ɂ���ۂɒn������ړ��̎��R�������Ȃ����邽�߂̒l
        float airResistance_ = 0.2f;//��C��R

        bool enableAutoTurn_ = true;//���x�ɍ��킹�Ď����ŉ�]�����邩

        bool hitWall_ = false;//�ǂɓ���������

        Vector3 center_ = { 0,1.2f,0 };

        float health_ = 10.0f;//�̗�
        float Max_Health = 10.0f;//�ő�̗�

        bool invincible_ = false;//���G

        //�O�I�v���ɂ��ړ��l �Ⴆ�Βn�`����ɂ�鉟���o���Ƃ�
        Vector3 externalFactorsMove_ = {};

        //�Ռ��Ȃǂ��󂯂ē����l
        Vector3 impactMove_ = {};
        //�Ռ����󂯂��ۂ̌����� ��������������
        float inpactDeccel_ = 3.0f;
        //�Ռ����󂯂��ۂɌ�������鑬�x�̍ő�l
        float inpactSpeedMax_ = 1000.0f;

        //�q�b�g�X�g�b�v
        float hitStopDuration_ = 0;//��������
        float hitStopOutTime_ = 0;//������t�F�[�h�A�E�g���Ă�����
        float hitStopTimer_ = 100;//���Ԍv���p

        bool isDead = false;
        
    };
}

