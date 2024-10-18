#pragma once
#include "ScriptComponent.h"
#include "MathHelper.h"

#include <string>

//キャラクターの基底クラス
//CharacterManagerからUpdate()を呼びだす
namespace AbyssEngine
{
    class SphereCollider;

    class Character : public ScriptComponent
    {
    public:
        using Tag = unsigned int;

        //タグの設定
        //敵味方の判別を行う
        static constexpr Tag Tag_Default    = 0x01;
        static constexpr Tag Tag_Player     = 0x01 << 1;
        static constexpr Tag Tag_Enemy      = 0x01 << 2;

        enum class DamageResult
        {
            Failed,     //失敗
            Success,    //成功
            FinalBlow   //とどめ
        };

        struct AttackParameter
        {
            float power_ = 1.0f;
            float knockback_ = 1.0f;
            Vector3 vector_ = {};
        };

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

        void DrawImGui()override;

        //複数アタッチ不可
        bool CanMultiple()override { return false; }

        bool Jump(const float& jumpPower);

        //指定方向に回転
        void TurnY(Vector3 dir,bool smooth = true/*なめらかに回転するか*/);
        void TurnY(Vector3 dir, const float& speed, bool smooth = true/*なめらかに回転するか*/);

        //ダメージを与える 攻撃が通ったならtrue
        virtual bool ApplyDamage(const AttackParameter& param,DamageResult* damageResult = nullptr);

        //死亡
        void Die();
        
        //ヒットストップ　持続時間とフェード開始時間
        void HitStop(float duration,float blendOutTime);

        //吹っ飛ばし そのまま吹き飛ばすベクトルを入れる
        void AddImpulse(Vector3 impulse);

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
        void SetOnGround(const bool& onGround) { onGround_ = onGround; }

        const float& GetAcceleration() const { return acceleration_; }
        void SetAcceleration(const float& accel) { acceleration_ = accel; }
        const float& GetDeceleration()const { return deceleration_; }
        void SetDeceleration(const float& decel) { deceleration_ = decel; }

        const AbyssEngine::Vector3& GetMoveVec() const { return moveVec_; }
        void SetMoveVec(const AbyssEngine::Vector3& vec) { moveVec_ = vec; }

        Vector3 GetCenterPos();

        const AbyssEngine::Vector3& GetExternalFactorsMove() const { return externalFactorsMove_; }
        //void SetExternalFactorsMove(const AbyssEngine::Vector3& move) { externalFactorsMove_ = move; } //セットは禁止！！！
        void AddExternalFactorsMove(const AbyssEngine::Vector3& move) { externalFactorsMove_ = externalFactorsMove_ + move; }

    protected:

        virtual void UpdateVelocity();//速力更新
        virtual void UpdateMove();

        //レイキャスト/スフィアキャスト両方に対応
        virtual void UpdateHorizontalMove();//地面に対して水平方向の移動処理による位置更新
        virtual void UpdateVerticalMove();//地面に対して垂直方向の移動処理による位置更新

        virtual void Landing();//着地

        float Gravity = -9.8f;

    private:
        void UpdateImpactMove();//ノックバックなどによって動かされる処理の更新

        void UpdateHitStop();//ヒットストップ更新処理　


    protected:
        Tag tag_ = Tag_Default;

        float weight_ = 1.0f;//重さ（重力、吹っ飛ばしの計算に使う）

        bool active_ = true;
        bool onGround_ = true;
        bool enableGravity_ = true;
        bool sphereCast_ = true;//地形判定にスフィアキャストを使用するか（falseの場合レイキャストを使用）

        float terrainRadius_ = 0.18f;//地形判定用の球の半径
        float terrainStepOffset_ = 0.14f;//スフィアキャストで始点にする位置を足元からどの程度上げるか
        float terrainCenterOffset_ = 0.3f;//中心
        float terrainSkinWidth_ = 0.05f;//キャスト量加算値

        float slopeLimit = 60.0f;//スロープ角度制限

        Vector3 moveVec_;//移動方向
        Vector3 velocity_;//速度
        float acceleration_ = 1.0f;//加速力
        float deceleration_ = 5.0f;//減速力
        float speedingDecel_ = 10.0f;//制限速度を超過してしまっているときに減速させられる速度

        float Max_Horizontal_Speed = 20.0f;//水平方向に対する最大速度
        float Max_Vertical_Speed = 20.0f;//縦方向に対する最大速度

        float baseRotSpeed_ = 300.0f;//回転速度
        float Max_Rot_Speed = 800.0f;//最大回転速度
        float Min_Rot_Speed = 300.0f;//最低回転速度

        float airborneCoefficient_ = 0.3f;//空中にいる際に地上よりも移動の自由を効かなくするための値
        float airResistance_ = 0.2f;//空気抵抗

        bool enableAutoTurn_ = true;//速度に合わせて自動で回転させるか

        bool hitWall_ = false;//壁に当たったか

        Vector3 center_ = { 0,1.2f,0 };

        float health_ = 10.0f;//体力
        float Max_Health = 10.0f;//最大体力

        bool invincible_ = false;//無敵

        //外的要因による移動値 例えば地形判定による押し出しとか
        Vector3 externalFactorsMove_ = {};

        //衝撃などを受けて動く値
        Vector3 impactMove_ = {};
        //衝撃を受けた際の減速力 ※小さい程強い
        float inpactDeccel_ = 3.0f;
        //衝撃を受けた際に減速される速度の最大値
        float inpactSpeedMax_ = 1000.0f;

        //ヒットストップ
        float hitStopDuration_ = 0;//持続時間
        float hitStopOutTime_ = 0;//いつからフェードアウトしていくか
        float hitStopTimer_ = 100;//時間計測用

        bool isDead = false;
        
    };
}

