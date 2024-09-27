#include "Collider.h"
#include "Actor.h"

using namespace AbyssEngine;

void Collider::Initialize(const std::shared_ptr<Actor>& actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();
}

void AbyssEngine::Collider::UpdateWorldMatrix()
{
    //���[���h�s��X�V
    transform_->CalcWorldMatrix();
}
