#pragma once
#include "Component.h"

namespace AbyssEngine
{
    //�Ǝ��̃R���|�[�l���g���J������ۂɌp�����Ƃ��Ďg�p����R���|�[�l���g
    //Update�����������Ă���
    class ScriptComponent : public Component
    {
    public:
        ScriptComponent() {}
        ~ScriptComponent() {}

        void Initialize(const std::shared_ptr<Actor>& actor)override;

        //�����ɓ������Ă���ꍇ�����ŌĂ΂��
        virtual void OnCollision(const std::shared_ptr<Collider>& collision, Collision::IntersectionResult result) {};

        virtual void UpdateBegin() {}
        virtual void UpdateBefore() {}
        virtual void Update() {}
        virtual void UpdateAfter() {}
        virtual void UpdateEnd() {}

        const bool& GetActive() const { return active_; }
        void SetActive(const bool& active) { active_ = active; }

    private:
        bool active_ = true;
    };
}

