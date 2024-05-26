#include "Animator.h"
#include "Actor.h"
#include "SkeletalMesh.h"
#include "Engine.h"

using namespace AbyssEngine;

void Animator::Initialize(const std::shared_ptr<Actor>& actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();
	skeletalMesh_ = actor->GetComponent<SkeletalMesh>();

	animatedNodes_ = skeletalMesh_.lock()->GetModel()->nodes_;
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

	model->Animate(animationClip_, timeStamp_, animatedNodes_, animationLoop_);
}
