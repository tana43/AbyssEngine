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

        bool DrawImGui()override;

        //�����A�^�b�`�s��
        bool CanMultiple()override { return false; }

        bool Jump(const float& jumpPower);

        //�w������ɉ�]
        void TurnY(Vector3 dir,bool smooth = true/*�Ȃ߂炩�ɉ�]���邩*/);
        void TurnY(Vector3 dir, const float& speed, bool smooth = true/*�Ȃ߂炩�ɉ�]���邩*/);

        //�_���[�W��^���� �U�����ʂ����Ȃ�true
        bool AddDamage(const float& damage,DamageResult& damageResult);

        //���S
        void Die();

        //�R���C�_�[��t����
        void AddTerrainCollider();  //�����o������p�R���C�_�[
        std::shared_ptr<SphereCollider> AddAttackCollider(Vector3 localPos, float radius,std::string name = "AtkCollider");   //�U������p�R���C�_�[
        std::shared_ptr<SphereCollider> AddHitCollider(Vector3 localPos, float radius, std::string name = "HitCollider");      //��炢����p�R���C�_�[

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

        Vector3 GetCenterPos();

    protected:

        virtual void UpdateVelocity();//���͍X�V
        virtual void UpdateMove();

        //���C�L���X�g/�X�t�B�A�L���X�g�����ɑΉ�
        virtual void UpdateHorizontalMove();//�n�ʂɑ΂��Đ��������̈ړ������ɂ��ʒu�X�V
        virtual void UpdateVerticalMove();//�n�ʂɑ΂��Đ��������̈ړ������ɂ��ʒu�X�V

        virtual void Landing();//���n
    public:
        constexpr static float Gravity = -9.8f;

    protected:
        Tag tag_ = Tag_Default;

        float weight_ = 1.0f;//�d���i�d�͂̌v�Z�Ɏg���j

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

        float Max_Horizontal_Speed = 20.0f;//���������ɑ΂���ő呬�x
        float Max_Vertical_Speed = 20.0f;//�c�����ɑ΂���ő呬�x

        float baseRotSpeed_ = 300.0f;//��]���x
        float Max_Rot_Speed = 800.0f;//�ő��]���x
        float Min_Rot_Speed = 300.0f;//�Œ��]���x

        float airborneCoefficient_ = 0.3f;//�󒆂ɂ���ۂɒn������ړ��̎��R�������Ȃ����邽�߂̒l
        float airResistance_ = 0.2f;//��C��R

        bool enableAutoTurn_ = true;//���x�ɍ��킹�Ď����ŉ�]�����邩

        bool hitWall_ = false;//�ǂɓ���������

        Vector3 center_ = { 0,0.3f,0 };

        float health_ = 10.0f;//�̗�
        float Max_Health = 10.0f;//�ő�̗�

        bool invincible_ = false;//���G
    };
}

