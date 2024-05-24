#pragma once
#include "Component.h"
#include "MathHelper.h"

//キャラクターの基底クラス
//CharacterManagerからUpdate()を呼びだす
namespace AbyssEngine
{
    class SkeletalMesh;

    class Character : public AbyssEngine::Component
    {
    public:
        virtual void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);
        virtual void Update() {}
        bool DrawImGui()override;

        //複数アタッチ不可
        bool CanMultiple()override { return false; }

    public:
        const bool& GetIsActive() const { return isActive_; }
        void SetIsActive(const bool& active) { isActive_ = active; }


    protected:
        //指定方向に回転
        void Turn(Vector3 dir,bool smooth = true/*なめらかに回転するか*/);

    public:
        constexpr static float Gravity = 1.0f;

    protected:
        bool isActive_ = true;

        Vector3 velocity_;//速度
        float acceleration_ = 1.0f;//加速力
        float deceleration_ = 5.0f;//減速力

        float Max_Speed = 10.0f;//最大速度

        float baseRotSpeed_ = 360.0f;//回転速度

        std::shared_ptr<AbyssEngine::SkeletalMesh> model_;
    };
}

