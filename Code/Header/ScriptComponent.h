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

        virtual void UpdateBegin() {}
        virtual void UpdateBefore() {}
        virtual void Update() {}
        virtual void UpdateAfter() {}
        virtual void UpdateEnd() {}

        const bool& GetIsActive() const { return isActive_; }
        void SetIsActive(const bool& active) { isActive_ = active; }

    private:
        bool isActive_ = true;
    };
}
