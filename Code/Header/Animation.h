#pragma once
#include <string>
#include "GeometricSubstance.h"

namespace AbyssEngine
{
    class SkeletalMesh;
    class GltfSkeletalMesh;
    class Animator;

    //アニメーションデータの基底クラス
    //シンプルな再生の場合このクラスをそのまま使う
    class Animation
    {
    public:
        Animation(SkeletalMesh* model,const std::string& name_,const int& index,bool loop = true);
        //Animation(SkeletalMesh* model,const std::string& name_,bool loop = true);
        ~Animation() {}

        //アニメーションの更新
        virtual void Initialize();
        virtual std::vector<GeometricSubstance::Node> UpdateAnimation(GltfSkeletalMesh* model, bool* animationFinished = nullptr);
        virtual void DrawImGui(Animator* animator);

    public:
        void SetLoopFlag(const bool& flag) { loopFlag_ = flag; }
        const bool& GetLoopFlag() const { return loopFlag_; }

        void SetAnimSpeed(const float& spd) { animSpeed_ = spd; }

        void SetTimeStamp(const float& time) { timeStamp_ = time; }
        const float GetTimeStamp() const { return timeStamp_; }

        void SetRootMotion(const bool& enable) { rootMotion_ = enable; }
        const bool& GetRootMotion() const { return rootMotion_; }

        void SetAnimator(Animator* animator) { animator_ = animator; }
        const Animator* GetAnimator() const { return animator_; }

        void SetRootMotionSpeed(const float& speed) { rootMotionSpeed_ = speed; }
        const float& GetRootMotionSpeed() const { return rootMotionSpeed_; }

        std::string name_;
        int animIndex_;//モデル本体が持っているこのモーションの要素数


        //それぞれのアニメーション情報を格納するボーン情報
        std::vector<GeometricSubstance::Node> animatedNodes_;

    protected: 
        void UpdateTime();

        bool loopFlag_ = true;
        float animSpeed_ = 1.0f;//各アニメーションごとの再生速度
        float timeStamp_ = 0.0f;

        //ルートモーション
        bool rootMotion_ = false;

        //ルートモーションによる移動の速度倍率
        float rootMotionSpeed_ = 1.0f;


        Animator* animator_ = nullptr;
    };


    //ブレンドスペース1D
    //BlendWeightが１つのみのモーションブレンド
    class AnimBlendSpace1D final : public Animation
    {
    public:
        AnimBlendSpace1D(SkeletalMesh* model, const std::string& name_, const int& index0, const int& index1);
        AnimBlendSpace1D(SkeletalMesh* model, AnimBlendSpace1D animData);
        ~AnimBlendSpace1D() {}

        //アニメーションの更新
        std::vector<GeometricSubstance::Node> UpdateAnimation(GltfSkeletalMesh* model, bool* animationFinished = nullptr)override;
        void DrawImGui(Animator* animator)override;

        //ブレンドするアニメーションの数を増やす
        void AddBlendAnimation(
            const int& index,   /*モデルの追加アニメーション要素番号*/
            const float& weight/*設定する重み*/);

        void SetMaxWeight(const float& weight) { maxWeight_ = weight; }
        void SetMinWeight(const float& weight) { minWeight_ = weight; }
        void SetBlendWeight(const float& weight) { blendWeight_ = weight; }

        const float& GetMaxWeight() const { return maxWeight_; }
        const float& GetMinWeight() const { return minWeight_; }
        const float& GetBlendWeight() const { return blendWeight_; }

        const float& GetLastBlendWeight() const { return lastBlendWeight_; }

    private:
        float blendWeight_ = 0.0f;//ブレンドの重さ
        float maxWeight_ = 1.0f;//ブレンドの最大値
        float minWeight_ = 0.0f;//ブレンドの最小値

        float lastBlendWeight_ = 0.0f;//前回のブレンドの重さ

        //ブレンドするアニメーション
        struct BlendAnimData
        {
            int index_;
            float weight_;//個々のモーションが持っているブレンドの重さ

            void operator=(const BlendAnimData& data)
            {
                index_ = data.index_;
                weight_ = data.weight_;
            }
        };
        std::vector<BlendAnimData> blendAnimDatas_;

        std::vector<GeometricSubstance::Node> blendAnimNodes_[2];
    };

    //ブレンドスペース2D
    //BlendWeightが２軸のモーションブレンド
    class AnimBlendSpace2D final : public Animation
    {
    public:
        enum class State
        {
            Idle,   //待機
            Move_F, //前進
            Move_R, //右移動
            Move_L, //左移動
            Move_B  //後退
        };

        enum class BlendSituation//どの程度のモーション数ブレンドが必要かを表す状態
        {
            None,//必要なし、待機のままでよい
            Once,//１回必要、待機と移動のブレンド
            Twice,//２回必要、移動と移動のブレンドをしたあと、さらに待機ともブレンドする
        };

        AnimBlendSpace2D(SkeletalMesh* model, const std::string& name_, const int& index, Vector2 weight = Vector2(0.0f,0.0f));
        ~AnimBlendSpace2D() {}

        //アニメーションの更新
        std::vector<GeometricSubstance::Node> UpdateAnimation(GltfSkeletalMesh* model ,bool* animationFinished = nullptr)override;
        void DrawImGui(Animator* animator)override;

        /// <summary>
        /// 追加順は、待機、右、左、後ろの順番になるように注意！！！
        /// なお待機のみこの関数を呼ぶ前に生成時で入っているはず...
        /// </summary>
        void AddBlendAnimation(
            const int& index,   /*モデルの追加アニメーション要素番号*/
            const Vector2& weight/*設定する重み*/);

        void SetBlendWeight(const Vector2& weight) { blendWeight_ = weight; }


        struct BlendAnimData
        {
            int index_;
            Vector2 weight_;
        };
        std::vector<BlendAnimData>& GetBlendAnims() { return blendAnimDatas_; }
        void SetBlendAnims(std::vector<BlendAnimData>& anims) { blendAnimDatas_ = anims; }

    private:
        Vector2 maxWeight_ = { 1.0f,1.0f };//ブレンドの最大値
        Vector2 minWeight_ = { -1.0f,-1.0f };//ブレンドの最小値
        Vector2 blendWeight_ = { 0.0f,0.0f };//ブレンドの重み
        
        std::vector<BlendAnimData> blendAnimDatas_;

        Vector2 lastBlendWeight_ = {0,0};//前回のブレンドの重さ

        std::vector<GeometricSubstance::Node> blendAnimNodes_[2];//ブレンドに使うノード
        std::vector<GeometricSubstance::Node> secondBlendAnimNodes_;//さらにブレンドするのに使うノード
    };

    //空中移動に上下移動のモーションをブレンドさせるために作った特化クラス
    //メンバ変数のブレンドスペースへの値の設定は別で処理をする必要がある
    class AnimBlendSpaceFlyMove final : public Animation
    {
    public:
        AnimBlendSpaceFlyMove(SkeletalMesh* model, const std::string& name_, AnimBlendSpace2D* blendSpace2D, AnimBlendSpace1D* blendSpace1D);
        ~AnimBlendSpaceFlyMove() {}

        std::vector<GeometricSubstance::Node> UpdateAnimation(GltfSkeletalMesh* model, bool* animationFinished = nullptr)override;

        void SetMoveVec(const AbyssEngine::Vector3& v) { moveVec_ = v; }
        const AbyssEngine::Vector3& GetMoveVec() const {return moveVec_; }

        AnimBlendSpace1D* GetBlendSpace1D() { return blendSpace1D_; }
        AnimBlendSpace2D* GetBlendSpace2D() { return blendSpace2D_; }
    private:
        AnimBlendSpace1D* blendSpace1D_;
        AnimBlendSpace2D* blendSpace2D_;

        //動いている方向(ここからブレンド比率を計算)
        AbyssEngine::Vector3 moveVec_;

        float lastBlendWeight_ = 0.0f;
    };

    class AnimAimIK : public Animation
    {
    public:
        AnimAimIK(SkeletalMesh* model, const std::string& name_);
        ~AnimAimIK() {}

        void DrawImGui(Animator* animator)override;

        std::vector<GeometricSubstance::Node> UpdateAnimation(GltfSkeletalMesh* model, bool* animationFinished = nullptr)override;

        void SetBaseAnimation(const int& index) { baseAnimationIndex_ = index; }

        void SetRootNodeName(const std::string& str) { rootNodeName_ = str; }
        void SetMidNodeName(const std::string& str) { midNodeName_ = str; }
        void SetTipNodeName(const std::string& str) { tipNodeName_ = str; }
        void SetIgnoreNodeName(const std::string& str) { ignoreNodeName_ = str; }
        void SetIgnoreNodeNameSecond(const std::string& str) { ignoreNodeNameSecond_ = str; }

        void SetTargetPosition(const Vector3& pos) { targetPosition_ = pos; }

        void SetRootInitRotation(const Vector3& rot) { rootInitRotation_ = { rot.x,rot.y,rot.x,1 }; }
        void SetHandInitRotation(const Vector3& rot) { handInitRotation_ = { rot.x,rot.y,rot.z,1 }; }

    private:
        //根本、中間、先端ノードの親子関係は連続しているか判定し、続いていなければ間のボーンを登録する
        //GeometricSubstance::Node* CheckDirectLineBones(GltfSkeletalMesh* model);

    private:
        //ターゲットの座標
        Vector3 targetPosition_;

        //Vector3 targetDirection_;

        //腕の伸ばし具合 0~1 ※1.5にして伸ばしすぎぐらいに
        float armExtension = 1.5f;

        //各ノードの名前　すぐに設定する必要がある
        std::string rootNodeName_ = "";
        std::string midNodeName_ = "";
        std::string tipNodeName_ = "";
        
        //　無視するノード名前　肩から手にかけて２つより多いボーンが存在するときに使う
        std::string ignoreNodeName_;
        std::string ignoreNodeNameSecond_;

        //無視ノードを中間ノードから上にするか下にするか
        bool isUpIgnoreNode_ = true;

        //腕以外のベースになるモーション
        int baseAnimationIndex_ = 0;

        //逆間接防止用のポールターゲット座標
        Vector3 poleLocalPosition_ = {0,-1.0f,0};

        //デバッグ用　初期回転値
        Vector4 rootInitRotation_ = {0,0,0,1};
        Vector4 handInitRotation_ = {0,0,0,1};
    };

    //エイムモーション
    class AnimAiming : public Animation
    {
    public:
        AnimAiming(SkeletalMesh* model, const std::string& name_, 
            const std::shared_ptr<AnimBlendSpace2D>& blendSpace2D,
            const std::shared_ptr<AnimAimIK>& rightHand,
            const std::shared_ptr<AnimAimIK>& leftHand);
        ~AnimAiming() {}

        std::vector<GeometricSubstance::Node> UpdateAnimation(GltfSkeletalMesh* model, bool* animationFinished = nullptr)override;

        std::shared_ptr<AnimBlendSpace2D>& GetBlendSpace2D() { return blendSpace2d_; }
        std::shared_ptr<AnimAimIK>& GetAimIkRight() { return aimIkRight_; }
        std::shared_ptr<AnimAimIK>& GetAimIkLeft() { return aimIkLeft_; }

        void DrawImGui(Animator* animator)override;

    private:
        std::shared_ptr<AnimBlendSpace2D> blendSpace2d_;
        std::shared_ptr<AnimAimIK> aimIkRight_;
        std::shared_ptr<AnimAimIK> aimIkLeft_;
    };
}