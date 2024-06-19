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
    lastBlendWeight_ = nextBlendWeight;

    //条件が完全に当てはまっているアニメーションはないか
    for (auto& animData : blendAnims_)
    {
        if (nextBlendWeight.x != animData.weight_.x)continue;
        if (nextBlendWeight.y != animData.weight_.y)continue;

        model->Animate(animData.index_, timeStamp, *animatedNodes_);

        return;
    }

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

    for (auto& animData : blendAnims_)
    {
        //現在のブレンド値からこのアニメーションのブレンド値を見たときの相対値
        Vector2 relative = animData.weight_ - nextBlendWeight;

        //各象限にアニメーションでデータを追加
        if (relative.x > 0 && relative.y > 0)
        {
            blendAnimDatas[0].emplace_back(animData);
        }
        else if (relative.x < 0 && relative.y > 0)
        {
            blendAnimDatas[1].emplace_back(animData);
        }
        else if(relative.x < 0 && relative.y < 0)
        {
            blendAnimDatas[2].emplace_back(animData);
        }
        else
        {
            blendAnimDatas[3].emplace_back(animData);
        }
    }

    //各象限で最も近いアニメーションを取得する
    BlendAnimData* animData[4];
    float distToNextWeight[4];
    int emptyCount = 0;//要素が０の象限の数
    //要素数０の象限をビット演算で登録　１ビット目を第一象限として考える
    for (int i = 0; i < 4; i++)
    {
        float nearestDist = FLT_MAX;

        if (blendAnimDatas[i].size() == 0)
        {
            emptyCount++;
            continue;
        }

        animData[i] = blendAnimDatas[i].at(0);

        for (auto& a : blendAnimDatas[i])
        {
            Vector2 relative = animData[i]->weight_ - nextBlendWeight;
            float dist = relative.Length();
            
            //距離を比較
            if (nearestDist > dist)
            {
                //最近値と登録されているアニメーションを再設定
                nearestDist = dist;
                animData[i] = a;
            }
        }

        //このアニメーションデータから最終的なブレンド率までの長さを設定
        distToNextWeight[i] = nearestDist;
    }

    //取得したアニメーションをブレンドしていく
    //まずデータからアニメーションを取得
    for (int i = 0; i < 4; i++)
    {
        if (animData[i]) 
        {
            model->Animate(animData[i]->index_, timeStamp, blendAnimNodes_[i]);
        }
    }

    //取得したアニメーションを使って順番にブレンドしていく
    //中身が空の象限をスキップする
    int secondNodesNo = 0;//ブレンドされたノードを入れる変数の要素番号
    for (int i = 0; i < 4; i++)
    {
        if (!animData[i])continue;
        for (int j = i + 1; j < 4; j++)
        {
            if (!animData[j])continue;

            //ブレンド率を算出する
            //少し前に計算したanimData[i]のブレンド率の距離とanimData[j]のブレンド率の距離の比率から求めていく
            float weight = distToNextWeight[j] / (distToNextWeight[i] + distToNextWeight[j]);
            model->BlendAnimations(blendAnimNodes_[i], blendAnimNodes_[j], weight, secondBlendAnimNodes_[secondNodesNo]);
            secondNodesNo++;
        }

        secondBlendAnimNodes_[secondNodesNo] = blendAnimNodes_[i];
    }

    //ブレンドした結果、２つのモーションが出来ているはずなので、さらにブレンドする
    //ただし空の象限の数が２つ以上あるときはブレンドされた結果は１つしかないのでそのままモデルに送る
    switch (emptyCount)
    {
    case 0:
    case 1:
        //0と1の時の処理は同じ

        //ブレンド率の計算

        //さらにブレンドし、モデルへ渡す
        model->BlendAnimations(secondBlendAnimNodes_[0], secondBlendAnimNodes_[1], 0, model->nodes_);

        break;
    case 2:
    case 3:
        //2と3の時の処理は同じ
        //そのままさっきブレンドした、もしくはそのままの結果をモデルへ渡す



        break;
    }
}

void AnimBlendSpace2D::DrawImGui(Animator* animator)
{
    if (ImGui::BeginMenu(name_.c_str()))
    {
        if (ImGui::Button("Play This Animation"))
        {
            animator->PlayAnimation(name_);
        }
        ImGui::SliderFloat("Blend Weight X", &blendWeight_.x, minWeight_.x, maxWeight_.x);
        ImGui::SliderFloat("Blend Weight Y", &blendWeight_.y, minWeight_.y, maxWeight_.y);
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
