#include "Helicopter.h"
#include "Actor.h"
#include "Animator.h"

#include "imgui/imgui.h"

using namespace AbyssEngine;

void Helicopter::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    Character::Initialize(actor);

    model_ = actor->AddComponent<SkeletalMesh>("./Assets/Models/Heli/SK_West_Heli_AH64D_ModelEditor.glb");
}

void Helicopter::Update()
{
    Character::Update();

    model_->GetAnimator()->GetAnimations().at(0)->SetAnimSpeed(motorPower_);
}

bool Helicopter::DrawImGui()
{
    ImGui::SliderFloat("Motor Power", &motorPower_, 0.0f, 2.0f);

    Character::DrawImGui();

    return true;
}
