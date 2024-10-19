#pragma once
#include "ScriptComponent.h"

#include <unordered_map>

namespace AbyssEngine
{
    class Character;
    class AttackCollider;

    enum class StaggerType
    {
        //High,
        Middle,
        //Low,
        None
    };

    //�U���f�[�^
    struct AttackData
    {
        float power_ = 0;//�U����
        float knockback_ = 0;//������΂���
        float duration_ = 1.0f;//��������
        float staggerValue_ = 0;//�X�^�b�O�l�i�Ђ�ݒl�̂悤�Ȃ��́j
        StaggerType staggerType_ = StaggerType::None;//�ǂ�ȋ��݂������邩

        int maxHits_ = 1;//�U�����A���q�b�g�ł���񐔁@�Œ�ł��P�͕K�v
        float hitInterval_ = 0.2f;//�U�����A���q�b�g����ۂɉ��b���ƂɍU����

        float hitStopDuration_ = 0.2f;//�U�������������ۂɃq�b�g�X�g�b�v���鎞��
        float hitStopOutTime_ = 0.05f;//�q�b�g�X�g�b�v�̃t�F�[�h�J�n����

        

        //������o��������R���C�_�[
        std::vector<std::weak_ptr<AttackCollider>> attackColliderList_;

        //Game:�q�b�g���Ƃ��������Ƃ�������

        AttackData& operator=(const AttackData& data);
    };

    class AttackerSystem : public ScriptComponent
    {
    public:
        AttackerSystem() {}
        ~AttackerSystem() {}

        void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;

        void Update()override;
        void DrawImGui()override;

        //�����ɓ������Ă���
        virtual void OnCollision(const std::shared_ptr<Collider>& collision, Collision::IntersectionResult result)override;

        //AttackData��ǉ�
        void RegistAttackData(std::string atkName,const AttackData atkData);

        //�R���C�_�[��ǉ�
        void RegistCollider(const std::shared_ptr<AttackCollider>& collider);

        //�w�肵�����O�̃f�[�^���g���čU�����J�n����
        void Attack(std::string atkName);

    private:
        void ApplyDamage(const std::shared_ptr<Character>& target);

        //���ݍU�����L���ȏ�Ԃ��̔��������
        void AttackEnabledUpdate();

    private:

        //�S�Ă̍U�������Ǘ����Ă���}�b�v
        std::unordered_map<std::string,AttackData> attackDataMap_;

        //���݂̍U��
        AttackData currentAttack_;

        //�U������x����������
        bool hit_ = false;
        //���݂̍U��������q�b�g������
        int hitCount_ = 0;
        //�U�����A���q�b�g�����Ƃ��̃q�b�g�Ԋu���v������^�C�}�[
        int hitIntervalTimer_ = 0;

        //�������Ԃ��v�����邽�߂̃^�C�}�[
        float durationTimer_ = 0;

        //�A�^�b�N�V�X�e����K�p������R���C�_�[
        std::vector<std::weak_ptr<AttackCollider>> attackColliderList_;

        //���ݍU�����L���ȏ�Ԃ�
        bool attackEnabled_ = false;
    };
}
