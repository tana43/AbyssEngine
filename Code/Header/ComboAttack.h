#pragma once

#include <memory>

namespace AbyssEngine
{
    class Animation;

    template<class T>
    class ComboAttack
    {
    public:
        ComboAttack(T* owner, const char* name,const std::shared_ptr<Animation>& animation) 
            : owner_(owner), name_(name) ,animation_(animation) {}
        virtual ~ComboAttack() {}

        virtual void Begin() = 0;
        virtual void Update() = 0;
        virtual void End() = 0;

        //�A�j���[�V�����̍Đ�
        virtual void PlayAnimation() = 0;

        virtual void DrawImGui() {}

    public:// �擾�E�ݒ�
        const char* GetName() { return name_; }

        // �^�C�}�[�֘A
        void SetTimer(float time) { timer_ = time; }
        float GetTimer()const { return timer_; }
        void SubtractTime(float time) { timer_ -= time; }

    protected:
        T* owner_;
        const char* name_;
        float timer_ = 0;    // �^�C�}�[

        //�A�j���[�V����
        std::weak_ptr<Animation> animation_;

        //�U����
        float attack_ = 1;

        //�U�����莝���J�n�E�I������
        float attackTime_[2] = {0,1};

        //��s���͎�t����
        float inputBufferingTime_ = 0.3f;

        //���̍s���փL�����Z���\�ɂȂ鎞��
        float cancelTime_ = 0.5f;

    };
}

