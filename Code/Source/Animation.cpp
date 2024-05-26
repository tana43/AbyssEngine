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

//現在のブレンドの重みから正に近いモーションと、負に近いモーションの二つを取得し、ブレンドする
void AnimBlendSpace1D::UpdateAnimation(GltfSkeletalMesh* model, float timeStamp)
{
    //ブレンドする２つのモーションを取る


    model->Animate(, timeStamp_, animatedNodes[0]);
    model->Animate(animationClip_ + 1, timeStamp_, animatedNodes[1]);
    
    model->BlendAnimations(animatedNodes[0], animatedNodes[1], blendWeight_, animatedNodes_);
}
