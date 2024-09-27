#include "BossMech.h"
#include "Actor.h"
#include "SkeletalMesh.h"

void BossMech::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    HumanoidWeapon::Initialize(actor);

    enableGravity_ = false;

    const auto& model = actor->AddComponent<AbyssEngine::SkeletalMesh>("./Assets/Models/Enemy/Boss/Mech_Idle.glb");
    model->GetAnimator()->AppendAnimations({
        "./Assets/Models/Enemy/Boss/Mech_Run.glb",
        "./Assets/Models/Enemy/Boss/Mech_Jump_Start.glb",
        "./Assets/Models/Enemy/Boss/Mech_Jump_Loop.glb",
        "./Assets/Models/Enemy/Boss/Mech_Jump_End.glb",
        "./Assets/Models/Enemy/Boss/Mech_CrouchingStart.glb",
        },
        {
            "Run",
            "Jump_Start",
            "Jump_Loop",
            "Jump_End",
            "Crouching"
        }
    );

    transform_->SetScaleFactor(35.0f);
}
