#pragma once
#include "Component.h"
#include "MathHelper.h"

//キャラクターの基底クラス
//CharacterManagerからUpdate()を呼びだす
namespace AbyssEngine
{
    class SkeletalMesh;
    class AnimBlendSpace1D;

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
        void TurnY(Vector3 dir,bool smooth = true/*なめらかに回転するか*/);

    public:
        constexpr static float Gravity = 1.0f;

    protected:
        bool isActive_ = true;

        Vector3 velocity_;//速度
        float acceleration_ = 1.0f;//加速力
        float deceleration_ = 5.0f;//減速力

        float Max_Speed = 20.0f;//最大速度

        float baseRotSpeed_ = 300.0f;//回転速度
        float Max_Rot_Speed = 800.0f;//最大回転速度
        float Min_Rot_Speed = 300.0f;//最低回転速度

        std::shared_ptr<AbyssEngine::SkeletalMesh> model_;

        AnimBlendSpace1D* moveAnimation_;
    };
}

