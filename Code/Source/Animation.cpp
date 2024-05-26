#include "Animation.h"
#include "SkeletalMesh.h"
#include "Engine.h"

using namespace AbyssEngine;

Animation::Animation(SkeletalMesh* model, const std::string& name, const int& index, bool loop) :
    name_(name),animIndex_(index),loopFlag_(loop)
{
    animatedNodes_ = &model->GetModel()->nodes_;
}

void Animation::UpdateAnimation(GltfSkeletalMesh* model, float timeStamp)
{
    timeStamp += (animSpeed_ - 1.0f) * Time::deltaTime_;
    model->Animate(animIndex_, timeStamp, *animatedNodes_, loopFlag_);
}

AnimBlendSpace1D::AnimBlendSpace1D(SkeletalMesh* model, const std::string& name, const int& index0, const int& index1)
    : Animation(model, name, index0, true) 
{
    blendAnimNodes_[0] = blendAnimNodes_[1] = *animatedNodes_;
}

//���݂̃u�����h�̏d�݂��琳�ɋ߂����[�V�����ƁA���ɋ߂����[�V�����̓���擾���A�u�����h����
void AnimBlendSpace1D::UpdateAnimation(GltfSkeletalMesh* model, float timeStamp)
{
    //�u�����h����Q�̃��[�V��������� blendWeight��[0]��[1]��weight�l�Ɏ��܂�͈͂̃A�j���[�V������T��
    BlendAnim blendAnims[2] = {blendAnims_[0],blendAnims_[1]};
    if (blendAnims_.size() >= 2)
    {
        for (int i = 2; i < blendAnims_.size(); i++)
        {
            if (blendAnims_[i].weight_ < blendWeight_)
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
    const float maxBlendWeight = blendAnims[1].weight_ - blendAnims[0].weight_;
    const float blendWeight = (blendWeight_ - blendAnims[0].weight_) / maxBlendWeight;

    //���[�V�����u�����h
    model->Animate(blendAnims[0].index_, timeStamp, animatedNodes_[0]);
    model->Animate(blendAnims[1].index_, timeStamp, animatedNodes_[1]);
    model->BlendAnimations(animatedNodes_[0], animatedNodes_[1], blendWeight, *animatedNodes_);
}

void AnimBlendSpace1D::AddBlendAnimation(const int& index, const float& weight)
{
    BlendAnim anim = { index,weight };
    blendAnims_.emplace_back(anim);
}
