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
        bool DrawImGui()override;

        //SkeletalMeshから呼ばれる初期化　通常の初期化ではできない処理
        void LatterInitialize(const std::shared_ptr<SkeletalMesh>& skeletalMesh);

        void AnimatorUpdate();

        //アニメーション再生　モーションの遷移時間を指定
        void PlayAnimation(const size_t& animIndex,float transTime = 0.1f);//要素数から検索
        void PlayAnimation(const std::string& animName,float transTime = 0.1f);//名前から検索
    private:
        void PlayAnimationCommon(const size_t& animIndex,float transTime);

    public:
        //アニメーションデータの再読み込み
        //void ReloadAnimation();

        //アニメーションアセットの追加
        void AppendAnimation(const std::string& filename,const std::string& motionName);
        void AppendAnimations(const std::vector<std::string>& filenames,const std::vector<std::string>& motionNames);
        AnimBlendSpace1D* AppendAnimation(AnimBlendSpace1D anim);
        AnimBlendSpace2D* AppendAnimation(AnimBlendSpace2D anim);

        std::vector<GeometricSubstance::Node>& GetAnimatedNodes() { return animatedNodes_; }
        void SetAnimatedNodes(const std::vector<GeometricSubstance::Node>& nodes) { animatedNodes_ = nodes; }

        void SetSkeletalMesh(const std::shared_ptr<SkeletalMesh>& mesh) { skeletalMesh_ = mesh; }
    private:
        //すべてのアニメーション
        std::vector<std::unique_ptr<Animation>> animations_;

        std::weak_ptr<SkeletalMesh> skeletalMesh_;

        float timeStamp_ = 0.0f;//タイマー
        float animationSpeed_ = 1.0f;//アニメーション再生速度

        size_t animationClip_ = 0;//再生中のアニメーション番号
        size_t nextAnimationClip_ = 0;//次に再生アニメーション番号
        //bool animationLoop_ = true;//ループ

        std::vector<GeometricSubstance::Node> animatedNodes_;//現在のアニメーション
        std::vector<GeometricSubstance::Node> nextAnimatedNodes_;//遷移先のアニメーション

        bool isTransitioningBlendAnim_ = false;//アニメーションの遷移のブレンド中か？
        float transitionTimer_ = 0;//アニメーション遷移タイマー
        float transitionTimeRequired_ = 0;//アニメーションの遷移に掛かる時間
    };

}

