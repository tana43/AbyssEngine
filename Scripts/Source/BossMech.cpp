#include "BossMech.h"
#include "Actor.h"
#include "SkeletalMesh.h"

void BossMech::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    HumanoidWeapon::Initialize(actor);

    enableGravity_ = false;

    actor->AddComponent<AbyssEngine::SkeletalMesh>("./Assets/Models/Enemy/Boss/Mech_Idle.glb");

    transform_->SetScaleFactor(35.0f);
}
