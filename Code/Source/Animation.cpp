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

//���݂̃u�����h�̏d�݂��琳�ɋ߂����[�V�����ƁA���ɋ߂����[�V�����̓���擾���A�u�����h����
void AnimBlendSpace1D::UpdateAnimation(GltfSkeletalMesh* model, float& timeStamp)
{
    //�u�����h�̑��x����
    const float blendMaxSpeed = 2.0f * Time::deltaTime_;
    const float blendSpeed = std::clamp(blendWeight_ - lastBlendWeight_, -blendMaxSpeed, blendMaxSpeed);

    //���ۂ̃u�����h�l�̌v�Z
    const float nextBlendWeight = std::clamp(lastBlendWeight_ + blendSpeed, minWeight_, maxWeight_);

    //�u�����h����Q�̃��[�V��������� blendWeight��[0]��[1]��weight�l�Ɏ��܂�͈͂̃A�j���[�V������T��
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
    
    //��̃��[�V�����f�[�^������ۂɕ⊮���Ɏg����l���v�Z
    //�}�ȃu�����h�̏d���̕ω���h���悤�ɑO��̒l����ϓ�����l�ɏ����݂���
    const float maxBlendWeight = blendAnims[1].weight_ - blendAnims[0].weight_;
    const float blendWeight = std::clamp((nextBlendWeight - blendAnims[0].weight_) / maxBlendWeight
                                        ,0.0f,1.0f);

    //���[�V�����u�����h
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
    //��������n����鏉�����[�V�����̂ݓo�^
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
    //�u�����h�̑��x����
    const float blendMaxSpeed = 2.0f * Time::deltaTime_;
    const Vector2 blendSpeed = {
        std::clamp(blendWeight_.x - lastBlendWeight_.x, -blendMaxSpeed, blendMaxSpeed),
        std::clamp(blendWeight_.y - lastBlendWeight_.y, -blendMaxSpeed, blendMaxSpeed)
    };

    //���ۂ̃u�����h�l�̌v�Z
    const Vector2 nextBlendWeight = {
        std::clamp(lastBlendWeight_.x + blendSpeed.x, minWeight_.x, maxWeight_.x),
        std::clamp(lastBlendWeight_.y + blendSpeed.y, minWeight_.y, maxWeight_.y)
    };
    lastBlendWeight_ = nextBlendWeight;

    //���������S�ɓ��Ă͂܂��Ă���A�j���[�V�����͂Ȃ���
    for (auto& animData : blendAnims_)
    {
        if (nextBlendWeight.x != animData.weight_.x)continue;
        if (nextBlendWeight.y != animData.weight_.y)continue;

        model->Animate(animData.index_, timeStamp, *animatedNodes_);

        return;
    }

    //Weight�l�����_�Ƃ��đ��`�l�ی��Ƀ��[�V�����f�[�^�𕪂���   
    //---------------------------------|------------------------------------
    //                                 |
    //                                 |
    //            ���ی�              |           ���ی�
    //                                 |
    //                                 |
    //---------------------------------|------------------------------------
    //                                 |
    //                                 |
    //            ��O�ی�              |           ��l�ی�
    //                                 |
    //                                 |
    //---------------------------------|------------------------------------
    std::vector<BlendAnimData*> blendAnimDatas[4];

    for (auto& animData : blendAnims_)
    {
        //���݂̃u�����h�l���炱�̃A�j���[�V�����̃u�����h�l�������Ƃ��̑��Βl
        Vector2 relative = animData.weight_ - nextBlendWeight;

        //�e�ی��ɃA�j���[�V�����Ńf�[�^��ǉ�
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

    //�e�ی��ōł��߂��A�j���[�V�������擾����
    BlendAnimData* animData[4];
    float distToNextWeight[4];
    int emptyCount = 0;//�v�f���O�̏ی��̐�
    //�v�f���O�̏ی����r�b�g���Z�œo�^�@�P�r�b�g�ڂ���ی��Ƃ��čl����
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
            
            //�������r
            if (nearestDist > dist)
            {
                //�ŋߒl�Ɠo�^����Ă���A�j���[�V�������Đݒ�
                nearestDist = dist;
                animData[i] = a;
            }
        }

        //���̃A�j���[�V�����f�[�^����ŏI�I�ȃu�����h���܂ł̒�����ݒ�
        distToNextWeight[i] = nearestDist;
    }

    //�擾�����A�j���[�V�������u�����h���Ă���
    //�܂��f�[�^����A�j���[�V�������擾
    for (int i = 0; i < 4; i++)
    {
        if (animData[i]) 
        {
            model->Animate(animData[i]->index_, timeStamp, blendAnimNodes_[i]);
        }
    }

    //�擾�����A�j���[�V�������g���ď��ԂɃu�����h���Ă���
    //���g����̏ی����X�L�b�v����
    int secondNodesNo = 0;//�u�����h���ꂽ�m�[�h������ϐ��̗v�f�ԍ�
    for (int i = 0; i < 4; i++)
    {
        if (!animData[i])continue;
        for (int j = i + 1; j < 4; j++)
        {
            if (!animData[j])continue;

            //�u�����h�����Z�o����
            //�����O�Ɍv�Z����animData[i]�̃u�����h���̋�����animData[j]�̃u�����h���̋����̔䗦���狁�߂Ă���
            float weight = distToNextWeight[j] / (distToNextWeight[i] + distToNextWeight[j]);
            model->BlendAnimations(blendAnimNodes_[i], blendAnimNodes_[j], weight, secondBlendAnimNodes_[secondNodesNo]);
            secondNodesNo++;
        }

        secondBlendAnimNodes_[secondNodesNo] = blendAnimNodes_[i];
    }

    //�u�����h�������ʁA�Q�̃��[�V�������o���Ă���͂��Ȃ̂ŁA����Ƀu�����h����
    //��������̏ی��̐����Q�ȏ゠��Ƃ��̓u�����h���ꂽ���ʂ͂P�����Ȃ��̂ł��̂܂܃��f���ɑ���
    switch (emptyCount)
    {
    case 0:
    case 1:
        //0��1�̎��̏����͓���

        //�u�����h���̌v�Z

        //����Ƀu�����h���A���f���֓n��
        model->BlendAnimations(secondBlendAnimNodes_[0], secondBlendAnimNodes_[1], 0, model->nodes_);

        break;
    case 2:
    case 3:
        //2��3�̎��̏����͓���
        //���̂܂܂������u�����h�����A�������͂��̂܂܂̌��ʂ����f���֓n��



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
