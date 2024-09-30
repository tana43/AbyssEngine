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

        //アニメーションの再生
        virtual void PlayAnimation() = 0;

        virtual void DrawImGui() {}

    public:// 取得・設定
        const char* GetName() { return name_; }

        // タイマー関連
        void SetTimer(float time) { timer_ = time; }
        float GetTimer()const { return timer_; }
        void SubtractTime(float time) { timer_ -= time; }

    protected:
        T* owner_;
        const char* name_;
        float timer_ = 0;    // タイマー

        //アニメーション
        std::weak_ptr<Animation> animation_;

        //攻撃力
        float attack_ = 1;

        //攻撃判定持続開始・終了時間
        float attackTime_[2] = {0,1};

        //先行入力受付時間
        float inputBufferingTime_ = 0.3f;

        //他の行動へキャンセル可能になる時間
        float cancelTime_ = 0.5f;

    };
}

