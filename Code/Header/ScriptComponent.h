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

        //何かに当たっている場合自動で呼ばれる
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

