#pragma once
#include "ScriptComponent.h"

#include <unordered_map>

namespace AbyssEngine
{
    class AttackerSystem : public ScriptComponent
    {
    public:
        //�U��
        struct AttackData
        {
            float power_ = 0;//�U����
            float duration_ = 1.0f;//��������
            float staggerValue_ = 0;//�X�^�b�O�l�i�Ђ�ݒl�̂悤�Ȃ��́j
            int maxHits = 1;//�U�����A���q�b�g�ł���񐔁@�Œ�ł��P�͕K�v

            float hitStop_ = 0;//�U�������������ۂɃq�b�g�X�g�b�v���鎞��

            //������o��������R���C�_�[
            std::vector<std::weak_ptr<AttackCollider>> attackColliderList_;
        };

    public:
        AttackerSystem() {}
        ~AttackerSystem() {}

        void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;

        void Update()override;

        //�����ɓ������Ă���
        virtual void OnCollision(const std::shared_ptr<Collider>& collision, Collision::IntersectionResult result)override;

        void RegistAttackData(std::string atkName,const AttackData atkData);

    private:

        //�S�Ă̍U�������Ǘ����Ă���}�b�v
        std::unordered_map<std::string,AttackData> attackDataMap_;

        //���݂̍U��
        AttackData currentAttack_;

        //�U������x����������
        bool hit_;
        //���݂̍U��������q�b�g������
        int hitCount_;

        //�������Ԃ��v�����邽�߂̃^�C�}�[
        float durationTimer_ = 0;

        //�A�^�b�N�V�X�e����K�p������R���C�_�[
        std::vector<std::weak_ptr<AttackCollider>> attackColliderList_;
    };
}
