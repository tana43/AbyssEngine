#pragma once
#include "ScriptComponent.h"
#include "Stage.h"

namespace AbyssEngine
{
    //��ѓ���R���|�[�l���g
    class Projectile : public ScriptComponent
    {
    public:
        Projectile() {}
        ~Projectile() {}

        void Initialize(const std::shared_ptr<Actor> actor);
        void Update();

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

    private:
        //�i�s����
        Vector3 direction_;

        //���x
        float speed_;

        //�����蔻��p�̔��a
        float radius_ = 0.1f;
    };
}

