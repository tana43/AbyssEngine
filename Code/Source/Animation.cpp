#include "Animation.h"
#include "SkeletalMesh.h"
#include "Engine.h"
#include "Animator.h"

#include <algorithm>

#include "imgui/imgui.h"

using namespace AbyssEngine;

Animation::Animation(SkeletalMesh* model, const std::string& name, const int& index, bool loop) :
    name_(name),animIndex_(index),loopFlag_(loop)
{
    animatedNodes_ = &model->GetAnimator()->GetAnimatedNodes();
}

void Animation::UpdateAnimation(GltfSkeletalMesh* model, float& timeStamp)
{
    timeStamp += (animSpeed_ - 1.0f) * Time::deltaTime_;
    model->Animate(animIndex_, timeStamp, *(animatedNodes_), loopFlag_);
}

void Animation::DrawImGui(Animator* animator)
{
    if (ImGui::BeginMenu(name_.c_str()))
    {
        if (ImGui::Button("Play This Animation"))
        {
            animator->PlayAnimation(name_);
        }

        ImGui::SliderFloat("Anim Speed", &animSpeed_, 0.0f, 2.0f);
        ImGui::Checkbox("Loop Flag", &loopFlag_);
        ImGui::Text(std::to_string(static_cast<long double>(animIndex_)).c_str());

        ImGui::EndMenu();
    }
}

AnimBlendSpace1D::AnimBlendSpace1D(SkeletalMesh* model, const std::string& name, const int& index0, const int& index1)
    : Animation(model, name, index0, true) 
{
    blendAnimNodes_[0] = blendAnimNodes_[1] = *animatedNodes_;
    blendAnims_.emplace_back(BlendAnimData(index0, 0.0f));
    blendAnims_.emplace_back(BlendAnimData(index1, 1.0f));
}

AnimBlendSpace1D::AnimBlendSpace1D(SkeletalMesh* model, AnimBlendSpace1D animData) : Animation(model,animData.name_,animData.animIndex_,true)
{
    blendAnimNodes_[0] = animData.blendAnimNodes_[0];
    blendAnimNodes_[1] = animData.blendAnimNodes_[1];
    blendAnims_ = animData.blendAnims_;
}

//現在のブレンドの重みから正に近いモーションと、負に近いモーションの二つを取得し、ブレンドする
void AnimBlendSpace1D::UpdateAnimation(GltfSkeletalMesh* model, float& timeStamp)
{
    //ブレンドの速度制限
    const float blendMaxSpeed = 2.0f * Time::deltaTime_;
    const float blendSpeed = std::clamp(blendWeight_ - lastBlendWeight_, -blendMaxSpeed, blendMaxSpeed);

    //実際のブレンド値の計算
    const float nextBlendWeight = std::clamp(lastBlendWeight_ + blendSpeed, minWeight_, maxWeight_);

    //ブレンドする２つのモーションを取る blendWeightが[0]と[1]のweight値に収まる範囲のアニメーションを探す
    BlendAnimData blendAnims[2] = {blendAnims_[0],blendAnims_[1]};
    if (blendAnims_.size() >= 2)
    {
        for (int i = 2; i < blendAnims_.size(); i++)
        {
            if (blendAnims_[i].weight_ < nextBlendWeight)
            {
                if (blendAnims_[i].weight_ > blendAnims[0].weight_)
                {
                    blendAnims[0] = blendAnims_[i];
                }
            }
            else
            {
                if (blendAnims_[i].weight_ < blendAnims[1].weight_)
                {
                    blendAnims[1] = blendAnims_[i];
                }
            }
        }
    }
    
    //二つのモーションデータから実際に補完率に使われる値を計算
    //急なブレンドの重さの変化を防ぐように前回の値から変動する値に上限を設ける
    const float maxBlendWeight = blendAnims[1].weight_ - blendAnims[0].weight_;
    const float blendWeight = std::clamp((nextBlendWeight - blendAnims[0].weight_) / maxBlendWeight
                                        ,0.0f,1.0f);

    //モーションブレンド
    model->Animate(blendAnims[0].index_, timeStamp, blendAnimNodes_[0]);
    model->Animate(blendAnims[1].index_, timeStamp, blendAnimNodes_[1]);
    model->BlendAnimations(blendAnimNodes_[0], blendAnimNodes_[1], blendWeight, *animatedNodes_);

    lastBlendWeight_ = nextBlendWeight;
}

void AnimBlendSpace1D::AddBlendAnimation(const int& index, const float& weight)
{
    BlendAnimData anim = { index,weight };
    blendAnims_.emplace_back(anim);
}

void AnimBlendSpace1D::DrawImGui(Animator* animator)
{
    if (ImGui::BeginMenu(name_.c_str()))
    {
        if (ImGui::Button("Play This Animation"))
        {
            animator->PlayAnimation(name_);
        }
        ImGui::SliderFloat("Blend Weight", &blendWeight_, minWeight_, maxWeight_);
        ImGui::SliderFloat("Anim Speed", &animSpeed_, 0.0f, 2.0f);
        ImGui::Checkbox("Loop Flag", &loopFlag_);
        ImGui::Text(std::to_string(static_cast<long double>(animIndex_)).c_str());

        ImGui::EndMenu();
    }
}

AnimBlendSpace2D::AnimBlendSpace2D(SkeletalMesh* model, const std::string& name, const int& index,Vector2 weight)
    : Animation(model,name,index)
{
    //引数から渡される初期モーションのみ登録
    blendAnims_.emplace_back(BlendAnimData(index,weight));

    for(auto& animNode : blendAnimNodes_)
    {
        animNode = *animatedNodes_;
    }
    for (auto& animNode : secondBlendAnimNodes_)
    {
        animNode = *animatedNodes_;
    }
}

void AnimBlendSpace2D::UpdateAnimation(GltfSkeletalMesh* model, float& timeStamp)
{
#if 0
    int loop = 0;
    while (loop < 2)
    {
        const float nextWeight = loop ? nextBlendWeight.y : nextBlendWeight.x;

        //X,Y軸それぞれでブレンドする２つのモーションを取る blendWeightが[0]と[1]のweight値に収まる範囲のアニメーションを探す
        BlendAnimData blendAnims[2] = { blendAnims_[0],blendAnims_[1] };
        if (blendAnims_.size() >= 2)
        {
            for (int i = 2; i < blendAnims_.size(); i++)
            {
                const float weight = loop ? blendAnims_[i].weight_.y : blendAnims_[i].weight_.x;
                if (weight < nextWeight)
                {
                    const float blendAnimWeight = loop ? blendAnims[0].weight_.y : blendAnims[0].weight_.x;
                    if (weight > blendAnimWeight)
                    {
                        blendAnims[0] = blendAnims_[i];
                    }
                }
                else
                {
                    const float blendAnimWeight = loop ? blendAnims[1].weight_.y : blendAnims[1].weight_.x;
                    if (weight < blendAnimWeight)
                    {
                        blendAnims[1] = blendAnims_[i];
                    }
                }
            }
        }

        //もしブレンドするデータのアニメインデックスが同じならモーションブレンド処理を省く
        if (blendAnims[0].index_ == blendAnims[1].index_)
        {
            model->Animate(blendAnims[0].index_, timeStamp, secondBlendAnimNodes_[loop]);
        }
        else
        {
            //二つのモーションデータから実際に補完率に使われる値を計算
            //急なブレンドの重さの変化を防ぐように前回の値から変動する値に上限を設ける
            const float blendAnimWeights[2] =
            {
                loop ? blendAnims[0].weight_.y : blendAnims[0].weight_.x,
                loop ? blendAnims[1].weight_.y : blendAnims[1].weight_.x
            };
            const float maxBlendWeight = blendAnimWeights[1] - blendAnimWeights[0];
            const float blendWeight = std::clamp((nextWeight - blendAnimWeights[0]) / maxBlendWeight
                , 0.0f, 1.0f);

            //モーションブレンド
            model->Animate(blendAnims[0].index_, timeStamp, blendAnimNodes_[0]);
            model->Animate(blendAnims[1].index_, timeStamp, blendAnimNodes_[1]);
            model->BlendAnimations(blendAnimNodes_[0], blendAnimNodes_[1], blendWeight, secondBlendAnimNodes_[loop]);
        }

        loop++;
    }

    //計算したx軸、y軸のブレンドモーションをさらにブレンドする
    model->BlendAnimations(secondBlendAnimNodes_[0], secondBlendAnimNodes_[1], blendWeight, secondBlendAnimNodes_[loop]);

#endif // 0
    //条件が完全に当てはまっているアニメーションはないか
    for (auto& animData : blendAnims_)
    {
        if (blendWeight_.x != animData.weight_.x)continue;
        if (blendWeight_.y != animData.weight_.y)continue;

        model->Animate(animData.index_, timeStamp,*animatedNodes_);

        return;
    }


    //ブレンドの速度制限
    const float blendMaxSpeed = 2.0f * Time::deltaTime_;
    const Vector2 blendSpeed = {
        std::clamp(blendWeight_.x - lastBlendWeight_.x, -blendMaxSpeed, blendMaxSpeed),
        std::clamp(blendWeight_.y - lastBlendWeight_.y, -blendMaxSpeed, blendMaxSpeed)
    };

    //実際のブレンド値の計算
    const Vector2 nextBlendWeight = {
        std::clamp(lastBlendWeight_.x + blendSpeed.x, minWeight_.x, maxWeight_.x),
        std::clamp(lastBlendWeight_.y + blendSpeed.y, minWeight_.y, maxWeight_.y)
    };

    //Weight値を原点として第一～四象限にモーションデータを分ける   
    //---------------------------------|------------------------------------
    //                                 |
    //                                 |
    //            第二象限              |           第一象限
    //                                 |
    //                                 |
    //---------------------------------|------------------------------------
    //                                 |
    //                                 |
    //            第三象限              |           第四象限
    //                                 |
    //                                 |
    //---------------------------------|------------------------------------
    std::vector<BlendAnimData*> blendAnimDatas[4];

    for (auto& anim : blendAnims_)
    {
        const Vector2 weight = anim.weight_;

        if (weight.y > nextBlendWeight.y)
        {
            //第一象限
            if (weight.x > nextBlendWeight.x)
            {
                blendAnimDatas[0].emplace_back(&anim);
                continue;
            }
            //第二象限
            else
            {
                blendAnimDatas[1].emplace_back(&anim);
                continue;
            }
        }
        else
        {
            //第三象限
            if (weight.x > nextBlendWeight.x)
            {
                blendAnimDatas[2].emplace_back(&anim);
                continue;
            }
            //第四象限
            else
            {
                blendAnimDatas[3].emplace_back(&anim);
                continue;
            }
        }
    }


    //各象限で最もweight値に近いアニメーションを取得する
    BlendAnimData* animData[4] = {};
    for (int i = 0; i < 4; i++)
    {
        if (blendAnimDatas[i].size() == 0)continue;

        animData[i] = blendAnimDatas[i].at(0);

        float nearestDist = FLT_MAX;
        int j = 0;
        for (auto& anim : blendAnimDatas[i])
        {
            //１ループは無効
            if (j == 0)
            {
                ++j;
                continue;
            }

            //weight値の距離を比較
            float dist = Vector2(anim->weight_ - nextBlendWeight).Length();
            if (dist < nearestDist)
            {
                animData[i] = anim;
                nearestDist = dist;
            }

            ++j;
        }
    }

    //4つのモーションデータから実際に補完率に使われる値を計算
    //急なブレンドの重さの変化を防ぐように前回の値から変動する値に上限を設ける

    const Vector2 maxBlendWeight = {
        (animData[0]->weight_.x > animData[3]->weight_.x) ? animData[0]->weight_.x : animData[3]->weight_.x,
        (animData[0]->weight_.y > animData[1]->weight_.y) ? animData[0]->weight_.y : animData[1]->weight_.y,
    };
    const Vector2 minBlendWeight = {
        (animData[1]->weight_.x > animData[1]->weight_.x) ? animData[2]->weight_.x : animData[3]->weight_.x,
        (animData[2]->weight_.y > animData[1]->weight_.y) ? animData[0]->weight_.y : animData[1]->weight_.y,
    };
    const Vector2 limitBlendWeight = maxBlendWeight - minBlendWeight;

    const Vector2 blendWeight = {
        std::clamp((nextBlendWeight.x - minBlendWeight.x) / limitBlendWeight.x, 0.0f, 1.0f),
        std::clamp((nextBlendWeight.y - minBlendWeight.y) / limitBlendWeight.y, 0.0f, 1.0f)
    };

    //モーションブレンド
    // //第一、第二象限のブレンドモーションと第三、第四象限のブレンドモーションを作成
    //その二つのモーションから最終的なモーションを算出
    //同一モーションを参照している場合計算を省く
    if (animData[0]->index_ == animData[1]->index_)
    {
        model->Animate(animData[0]->index_, timeStamp, secondBlendAnimNodes_[0]);
    }
    else
    {
        model->Animate(animData[0]->index_, timeStamp, blendAnimNodes_[0]);
        model->Animate(animData[1]->index_, timeStamp, blendAnimNodes_[1]);
        model->BlendAnimations(blendAnimNodes_[0], blendAnimNodes_[1], blendWeight.x, secondBlendAnimNodes_[0]);
    }

    if (animData[2]->index_ == animData[3]->index_)
    {
        model->Animate(animData[2]->index_, timeStamp, secondBlendAnimNodes_[1]);
    }
    else
    {
        model->Animate(animData[2]->index_, timeStamp, blendAnimNodes_[0]);
        model->Animate(animData[3]->index_, timeStamp, blendAnimNodes_[1]);
        model->BlendAnimations(blendAnimNodes_[0], blendAnimNodes_[1], blendWeight.x, secondBlendAnimNodes_[1]);
    }

    model->BlendAnimations(secondBlendAnimNodes_[0],secondBlendAnimNodes_[1],blendWeight.y,*animatedNodes_);

    lastBlendWeight_ = nextBlendWeight;
}

void AnimBlendSpace2D::DrawImGui(Animator* animator)
{
    if (ImGui::BeginMenu(name_.c_str()))
    {
        if (ImGui::Button("Play This Animation"))
        {
            animator->PlayAnimation(name_);
        }
        ImGui::SliderFloat2("Blend Weight", &blendWeight_.x, minWeight_.x, maxWeight_.x);
        ImGui::SliderFloat("Anim Speed", &animSpeed_, 0.0f, 2.0f);
        ImGui::Checkbox("Loop Flag", &loopFlag_);
        ImGui::Text(std::to_string(static_cast<long double>(animIndex_)).c_str());

        ImGui::EndMenu();
    }
}

void AnimBlendSpace2D::AddBlendAnimation(const int& index, const Vector2& weight)
{
    auto& a = blendAnims_.emplace_back(BlendAnimData(index, weight));
}
