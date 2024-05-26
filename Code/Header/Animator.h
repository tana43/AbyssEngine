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

