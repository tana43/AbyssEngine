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
        virtual std::vector<GeometricSubstance::Node> UpdateAnimation(GltfSkeletalMesh* model);
        virtual void DrawImGui(Animator* animator);

    public:
        void SetLoopFlag(const bool& flag) { loopFlag_ = flag; }

        std::string name_;
        int animIndex_;//モデル本体が持っているこのモーションの要素数
    protected: 
        void UpdateTime();

        bool loopFlag_ = true;
        float animSpeed_ = 1.0f;//各アニメーションごとの再生速度
        float timeStamp_ = 0.0f;

        //モデルの持つノードへアクセスするためのポインタ
        std::vector<GeometricSubstance::Node> animatedNodes_;
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
        std::vector<GeometricSubstance::Node> UpdateAnimation(GltfSkeletalMesh* model)override;
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
        std::vector<GeometricSubstance::Node> UpdateAnimation(GltfSkeletalMesh* model)override;
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
        AnimBlendSpaceFlyMove(SkeletalMesh* model, const std::string& name, AnimBlendSpace2D* blendSpace2D, AnimBlendSpace1D* blendSpace1D);
        ~AnimBlendSpaceFlyMove() {}

        std::vector<GeometricSubstance::Node> UpdateAnimation(GltfSkeletalMesh* model)override;

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
}