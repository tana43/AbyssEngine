#include "TestSceneSecond.h"
#include "Helicopter.h"
#include "Camera.h"
#include "BossMech.h"
#include "Vitesse.h"

using namespace AbyssEngine;

void TestSceneSecond::Initialize()
{
    const auto& camera = InstanceActor("DebugCamera_TestS");
    camera->AddComponent<AbyssEngine::Camera>();

    const auto& heli = InstanceActor("Helicopter_TestS");
    heli->AddComponent<Helicopter>();

    const auto& boss = InstanceActor("Boss_Mech_Test");
    boss->AddComponent<BossMech>();

    const auto& vitesse = InstanceActor("Vitesse");
    const auto& vc = vitesse->AddComponent<Vitesse>();
    //ƒJƒƒ‰‚ð•ÏX
    Camera::ChangeMainCamera(vc->GetCamera().get());
    vc->ChangeState(Vitesse::ActionState::TakeOff);
}
