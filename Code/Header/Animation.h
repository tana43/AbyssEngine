#pragma once
#include <string>
#include "GeometricSubstance.h"

namespace AbyssEngine
{
    class SkeletalMesh;
    class GltfSkeletalMesh;

    //アニメーションデータの基底クラス
    //シンプルな再生の場合このクラスをそのまま使う
    class Animation
    {
    public:
        Animation(SkeletalMesh* model,const std::string& name,const int& index,bool loop = false);
        ~Animation() {}

        //アニメーションの更新
        virtual void UpdateAnimation(GltfSkeletalMesh* model,float& timeStamp);

    protected:
        std::string name_;
        int animIndex_;

        float animSpeed_ = 1.0f;//各アニメーションごとの再生速度
        bool loopFlag_;

        //モデルの持つノードへアクセスするためのポインタ
        std::vector<GeometricSubstance::Node>* animatedNodes_;
    };

    //ブレンドスペース1D
    //BlendWeightが１つのみのモーションブレンド
    class AnimBlendSpace1D final : public Animation
    {
    public:
        AnimBlendSpace1D(SkeletalMesh* model, const std::string& name, const int& index0, const int& index1);
        ~AnimBlendSpace1D() {}

        //アニメーションの更新
        void UpdateAnimation(GltfSkeletalMesh* model, float& timeStamp);

        //ブレンドするアニメーションの数を増やす
        void AddBlendAnimation(
            const int& index,   /*モデルの追加アニメーション要素番号*/
            const float& weight/*設定する重み*/);

        void SetMaxWeight(const float& weight) { maxWeight_ = weight; }
        void SetMinWeight(const float& weight) { minWeight_ = weight; }

    private:
        float blendWeight_ = 0.0f;//ブレンドの重さ
        float maxWeight_ = 1.0f;//ブレンドの最大値
        float minWeight_ = 0.0f;//ブレンドの最小値

        //ブレンドするアニメーション
        struct BlendAnim
        {
            int index_;
            float weight_;//個々のモーションが持っているブレンドの重さ
        };
        std::vector<BlendAnim> blendAnims_;

        std::vector<GeometricSubstance::Node> blendAnimNodes_[2];
    };
}

