#include "Animator.h"
#include "Actor.h"
#include "SkeletalMesh.h"
#include "Engine.h"

using namespace AbyssEngine;

void Animator::Initialize(const std::shared_ptr<Actor>& actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();

	//��������ɃR���|�[�l���g�̓A�^�b�`�����̂�GetComponent�͋@�\���Ȃ�
	//skeletalMesh_ = actor->GetComponent<SkeletalMesh>();
	//animatedNodes_ = skeletalMesh_.lock()->GetModel()->nodes_;
}

void Animator::LatterInitialize(const std::shared_ptr<SkeletalMesh>& skeletalMesh)
{
	skeletalMesh_ = skeletalMesh;
	animatedNodes_ = skeletalMesh->GetModel()->nodes_;

	//�������[�V�������A�j���[�^�[�ɒǉ�
	//���Ȃ炸�O�Ԗڂ̃��[�V�����͑ҋ@�Ɖ���
	Animation anim(skeletalMesh.get(), "Idle", 0,true);
	animations_.emplace_back(anim);
}

void Animator::AnimatorUpdate()
{
	if (!skeletalMesh_.lock())
	{
		_ASSERT_EXPR("�|�C���^�悪����܂���", false);
		return;
	}
	const auto& model = skeletalMesh_.lock()->GetModel();

	//���ԍX�V
	timeStamp_ += Time::deltaTime_ * animationSpeed_;

	//model->Animate(animationClip_, timeStamp_, animatedNodes_, animationLoop_);
	animations_[animationClip_].UpdateAnimation(model,timeStamp_);
}

void Animator::PlayAnimation(size_t animIndex, bool loop)
{
	_ASSERT_EXPR(animIndex < skeletalMesh_.lock()->GetModel()->animations_.size(), u8"�w��̃A�j���[�V������������܂���");

	timeStamp_ = 0.0f;

	animationClip_ = animIndex;
	animationLoop_ = loop;
}

//void Animator::ReloadAnimation()
//{
//
//}

Animation& Animator::AppendAnimation(const std::string& filename, const std::string& motionName)
{
	const auto& model = skeletalMesh_.lock();
	_ASSERT_EXPR(model, "");
	
	model->GetModel()->AppendAnimation(filename);
	animatedNodes_ = model->GetModel()->nodes_;
	
	Animation anim = Animation(model.get(), motionName, animations_.size());
	return animations_.emplace_back(anim);
}

void Animator::AppendAnimations(const std::vector<std::string>& filenames, const std::vector<std::string>& motionNames)
{
	const auto& model = skeletalMesh_.lock();
	if (!model)return;

	model->GetModel()->AppendAnimations(filenames);
	for (int i = 0; i < filenames.size(); i++)
	{
		Animation anim = Animation(model.get(), motionNames[i], animations_.size());
		animations_.emplace_back(anim);
	}
}
