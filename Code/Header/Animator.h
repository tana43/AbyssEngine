#pragma once
#include "Animation.h"
#include "Component.h"

namespace AbyssEngine
{
    class SkeletalMesh;

    class Animator final : public Component
    {
    public:
        Animator() {}
        ~Animator() {}

        void Initialize(const std::shared_ptr<Actor>& actor)override;

        void AnimatorUpdate();

        void PlayAnimation(size_t animIndex, bool loop = true);

        //アニメーションデータの再読み込み
        void ReloadAnimation();

        //アニメーションアセットの追加
        Animation& AppendAnimation(const std::string& filename,const std::string& motionName);
        void AppendAnimations(const std::vector<std::string>& filenames,const std::vector<std::string>& motionNames);

    private:
        //すべてのアニメーション
        std::vector<Animation> animations_;

        std::weak_ptr<SkeletalMesh> skeletalMesh_;

        float timeStamp_ = 0.0f;//タイマー
        float animationSpeed_ = 1.0f;//アニメーション再生速度

        size_t animationClip_ = 0;//再生中のアニメーション要素番号
        bool animationLoop_ = true;//ループ

        std::vector<GeometricSubstance::Node> animatedNodes_;
    };

}

