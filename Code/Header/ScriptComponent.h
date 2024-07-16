#pragma once
#include "Component.h"

namespace AbyssEngine
{
    //独自のコンポーネントを開発する際に継承元として使用するコンポーネント
    //Update処理を持っている
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

