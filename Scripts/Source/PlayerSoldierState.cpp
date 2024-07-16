#include "PlayerSoldierState.h"
#include "PlayerSoldier.h"
#include "Vitesse.h"
#include "Transform.h"
#include "Input.h"

using namespace AbyssEngine;

void SoldierState::Move::Initialize()
{

}

void SoldierState::Move::Update()
{
    owner_->InputMove();

    //ƒ”ƒBƒeƒX‚Ö“‹æ
    if (Input::GameSupport::GetBoardingButton())
    {
        owner_->BoardingTheVitesse();
    }
}

void SoldierState::Move::Finalize()
{

}
