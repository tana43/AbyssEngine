#pragma once
#include "ScriptComponent.h"
#include "MathHelper.h"

//キャラクターの基底クラス
//CharacterManagerからUpdate()を呼びだす
namespace AbyssEngine
{
    class Character : public ScriptComponent
    {
    public:
        using Tag = unsigned int;

        //タグの設定
        //敵味方の判別を行う
        static constexpr Tag Tag_Default    = 0x01;
        static constexpr Tag Tag_Player     = 0x01 << 1;
        static constexpr Tag Tag_Enemy      = 0x01 << 2;

        Tag GetTag() const { return tag_; }
        void ReplaceTag(Tag t) { tag_ = t; }
        void AddTag(Tag t)
        {
            tag_ |= t;
        }

    public:
        virtual void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
        virtual void Update()override;
        //virtual void DelayedUpdate() {}//描画時の行列更新などが終わってから

        bool DrawImGui()override;

        //複数アタッチ不可
        bool CanMultiple()override { return false; }

        bool Jump(const float& jumpPower);

        //指定方向に回転
        void TurnY(Vector3 dir,bool smooth = true/*なめらかに回転するか*/);
        void TurnY(Vector3 dir, const float& speed, bool smooth = true/*なめらかに回転するか*/);


    public:
        const bool& GetIsActive() const { return active_; }
        void SetIsActive(const bool& active) { active_ = active; }

        const Vector3& GetVelocity() { return velocity_; }
        void SetVelocity(const Vector3& velo) { velocity_ = velo; }
        void SetVelocityX(const float& x) { velocity_.x = x; }
        void SetVelocityY(const float& y) { velocity_.y = y; }
        void SetVelocityZ(const float& z) { velocity_.z = z; }

        const float& GetMaxHorizontalSpeed() const { return Max_Horizontal_Speed; }
        void SetMaxHorizontalSpeed(const float& speed) { Max_Horizontal_Speed = speed; }
        const float& GetMaxVerticalSpeed()const { return Max_Vertical_Speed; }
        void SetMaxVerticalSpeed(const float& speed) { Max_Vertical_Speed = speed; }

        void SetEnableAutoTurn(const bool& flag) { enableAutoTurn_ = flag; }

        const bool& GetOnGround() const { return onGround_; }

    protected:
        

        virtual void Move();//移動処理

        virtual void UpdateVelocity();//速力更新
        virtual void UpdateMove();
        virtual void UpdateHorizontalMove();//地面に対して水平方向の移動処理による位置更新
        virtual void UpdateVerticalMove();//地面に対して垂直方向の移動処理による位置更新

        virtual void Landing();//着地
    public:
        constexpr static float Gravity = -9.8f;

    protected:
        Tag tag_ = Tag_Default;

        float weight_ = 1.0f;//重さ（重力の計算に使う）

        bool active_ = true;
        bool onGround_ = true;
        bool enableGravity_ = true;

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

        bool enableAutoTurn_ = true;//速度に合わせて自動で回転させるか

        float headHeight_ = 1.7f;//頭の高さ
    };
}

