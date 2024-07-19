#include "Human.h"
#include "SkeletalMesh.h"

void Human::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    AbyssEngine::Character::Initialize(actor);
}

bool Human::DrawImGui()
{
    return AbyssEngine::Character::DrawImGui();
}

const std::shared_ptr<AbyssEngine::Animator>& Human::GetAnimator()
{
    return model_->GetAnimator();
}
