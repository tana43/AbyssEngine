#pragma once
#include <string>

namespace AbyssEngine
{
    //アニメーションデータの基底クラス
    //シンプルな再生の場合このクラスをそのまま使う
    class Animation
    {
    public:
        Animation(const std::string& name,const int& index,bool loop = true);
        ~Animation() {}

        //アニメーションの更新
        virtual void UpdateAnimation();

    protected:
        std::string name_;
        int animIndex_;

        float animSpeed_ = 1.0f;//各アニメーションごとの再生速度
        bool loopFlag_ = true;
    };

    //ブレンドスペース1D
    //BlendWeightが１つのみのモーションブレンド
    class BlendSpace1D final : public Animation
    {
    public:
        BlendSpace1D(const std::string& name, const int& index,bool loop) : Animation(name, index,loop) {}
        ~BlendSpace1D() {}

    private:
        float blendWeight_ = 0.0f;
    };
}

