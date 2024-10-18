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
        void DrawImGui()override;

        //SkeletalMeshから呼ばれる初期化　通常の初期化ではできない処理
        void LatterInitialize(const std::shared_ptr<SkeletalMesh>& skeletalMesh);

        void AnimatorUpdate();

        //アニメーション再生　モーションの遷移時間を指定　無かった場合はメンバ変数の値が代入される
        void PlayAnimation(const size_t& animIndex,float* transTime = nullptr,float startTime = 0.0f);//要素数から検索
        void PlayAnimation(const std::string& animName,float* transTime = nullptr,float startTime = 0.0f);//名前から検索
    private:
        void PlayAnimationCommon(const size_t& animIndex,float transTime,float startTime = 0.0f);

    public:
        //アニメーションデータの再読み込み
        //void ReloadAnimation();

        //アニメーションアセットの追加
        void AppendAnimation(const std::string& filename,const std::string& motionName);
        void AppendAnimations(const std::vector<std::string>& filenames,const std::vector<std::string>& motionNames);
        AnimBlendSpace1D* AppendAnimation(AnimBlendSpace1D anim);
        AnimBlendSpace2D* AppendAnimation(AnimBlendSpace2D anim);
        AnimBlendSpaceFlyMove* AppendAnimation(AnimBlendSpaceFlyMove anim);

        std::vector<GeometricSubstance::Node>& GetAnimatedNodes() { return animatedNodes_; }
        void SetAnimatedNodes(const std::vector<GeometricSubstance::Node>& nodes) { animatedNodes_ = nodes; }

        void SetSkeletalMesh(const std::shared_ptr<SkeletalMesh>& mesh) { skeletalMesh_ = mesh; }

        //現在のアニメーションを取得
        const int GetCurrentAnimClip() const { return static_cast<int>(animationClip_); }
        //次のアニメーションを取得
        const int GetNextAnimClip() const { return static_cast<int>(nextAnimationClip_); }

        std::vector<Animation*> GetAnimations();

        const bool& GetAnimationFinished() const { return isFinished_; }
        
        //ルートジョイントのインデックスを設定
        //※ルートモーションに必須
        void SetRootJointIndex(const int& index) { rootJointIndex_ = index; }

        void SetEnableRootMotion(const bool& enable) { enableRootMotion_ = enable; }

        const Vector3& GetRootMotionMove() const { return rootMotionMove_; }
        void SetRootMotionMove(const Vector3& move) { rootMotionMove_ = move; }

        void SetAnimationTransTime(const float& time) { animationTransTime_ = time; }
    private:
        //すべてのアニメーション
        std::vector<std::unique_ptr<Animation>> animations_;

        std::weak_ptr<SkeletalMesh> skeletalMesh_;

        float timeStamp_ = 0.0f;//タイマー
        float animationSpeed_ = 1.0f;//アニメーション再生速度

        float animationTransTime_ = 0.1f;//アニメーションの遷移時間

        size_t animationClip_ = 0;//再生中のアニメーション番号
        size_t nextAnimationClip_ = -1;//次に再生アニメーション番号
        //bool animationLoop_ = true;//ループ

        std::vector<GeometricSubstance::Node> animatedNodes_;//現在のアニメーション
        std::vector<GeometricSubstance::Node> nextAnimatedNodes_;//遷移先のアニメーション

        bool isTransitioningBlendAnim_ = false;//アニメーションの遷移のブレンド中か？
        float transitionTimer_ = 0;//アニメーション遷移タイマー
        float transitionTimeRequired_ = 0;//アニメーションの遷移に掛かる時間

        bool isFinished_;//アニメーションの再生が終わっているか

        bool enableRootMotion_ = false;//ルートモーション
        std::vector<GeometricSubstance::Node> zeroAnimatedNodes_;//初期姿勢
        bool isSetZeroAnimatedNodes_ = false;//初期姿勢を設定しているか
        int rootJointIndex_ = 0;//ルートボーン（最も上の階層にあるボーン）のインデックス
        Vector3 previousPosition_;//前回の位置

        //ルートモーションによって移動する値(別途リセットが必要)
        Vector3 rootMotionMove_;

    };

}

