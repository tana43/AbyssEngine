#pragma once
#include "ScriptComponent.h"
#include "MathHelper.h"

//キャラクターの基底クラス
//CharacterManagerからUpdate()を呼びだす
namespace AbyssEngine
{
    class SkeletalMesh;
    class Animator;
    class AnimBlendSpace1D;

    class Character : public ScriptComponent
    {
    public:
        virtual void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);
        virtual void Update()override {}
        //virtual void DelayedUpdate() {}//描画時の行列更新などが終わってから

        bool DrawImGui()override;

        //複数アタッチ不可
        bool CanMultiple()override { return false; }

        void Jump(const float& jumpPower);

    public:
        const bool& GetIsActive() const { return isActive_; }
        void SetIsActive(const bool& active) { isActive_ = active; }

        const std::shared_ptr<SkeletalMesh>& GetModel() { return model_; }
        const std::shared_ptr<Animator>& GetAnimator();

        const Vector3& GetVelocity() { return velocity_; }
        void SetVelocity(const Vector3& velo) { velocity_ = velo; }
        void SetVelocityX(const float& x) { velocity_.x = x; }
        void SetVelocityY(const float& y) { velocity_.y = y; }
        void SetVelocityZ(const float& z) { velocity_.z = z; }

        const float GetMaxHorizontalSpeed() const { return Max_Horizontal_Speed; }
        const float GetMaxVerticalSpeed()const { return Max_Vertical_Speed; }

    protected:
        //指定方向に回転
        void TurnY(Vector3 dir,bool smooth = true/*なめらかに回転するか*/);

        virtual void Move();//移動処理

        virtual void UpdateVelocity();//速力更新
        virtual void UpdateMove();
        virtual void UpdateHorizontalMove();//地面に対して水平方向の移動処理による位置更新
        virtual void UpdateVerticalMove();//地面に対して垂直方向の移動処理による位置更新

        virtual void Landing();//着地
    public:
        constexpr static float Gravity = -9.8f;

    protected:
        float weight_ = 1.0f;//重さ（重力の計算に使う）

        bool isActive_ = true;
        bool onGround_ = true;

        Vector3 moveVec_;//移動方向
        Vector3 velocity_;//速度
        float acceleration_ = 1.0f;//加速力
        float deceleration_ = 5.0f;//減速力

        float Max_Horizontal_Speed = 20.0f;//水平方向に対する最大速度
        float Max_Vertical_Speed = 20.0f;//縦方向に対する最大速度

        float baseRotSpeed_ = 300.0f;//回転速度
        float Max_Rot_Speed = 800.0f;//最大回転速度
        float Min_Rot_Speed = 300.0f;//最低回転速度

        float airborneCoefficient_ = 0.3f;//空中にいる際に地上よりも移動の自由を効かなくするための値
        float airResistance_ = 0.2f;//空気抵抗

        std::shared_ptr<AbyssEngine::SkeletalMesh> model_;

        AnimBlendSpace1D* moveAnimation_;//走り移動
    };
}

