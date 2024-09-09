#pragma once
#include "ScriptComponent.h"
#include "Stage.h"

namespace AbyssEngine
{
    class SphereCollider;

    //��ѓ���R���|�[�l���g
    class Projectile : public ScriptComponent
    {
    public:
        Projectile() {}
        ~Projectile() {}

        void Initialize(const std::shared_ptr<Actor>& actor);
        void Update()override;

        /// <summary>
        /// ���C�L���X�g
        /// �����Ɠ����ɒn�`�ƃ��C�L���X�g�����Ă����A�e���G�ɓ�����Ȃ������Ƃ��A
        /// �ǂ��̒n�`�Ƀq�b�g���邩�����炩���ߎZ�o���Ă���
        /// </summary>
        /// <param name="stage">����ΏۂɂȂ�X�e�[�W�R���|�[�l���g</param>
        /// <param name="distance">���C�̒��������A�����ヌ�C�̃q�b�g�ʒu���������ĕԂ��Ă���</param>
        /// <returns>���C�͓���������</returns>
        bool RayCast(const std::shared_ptr<Stage>& stage,
            float& distance,
            Vector3& hitPosition);

    public:
        void SetDirection(const Vector3& dir) { direction_ = dir; }

        const std::shared_ptr<SphereCollider>& GetCollider() const { return collider_; }

        void SetRadius(const float& radius);

    protected:
        //�i�s����
        Vector3 direction_;

        //���x
        float speed_ = 50.0f;

        //�����蔻��p�̔��a
        float radius_ = 0.1f;

        //����
        float flashLifespan_ = 10.0f;

        //������
        std::shared_ptr<SphereCollider> collider_;
    };
}

