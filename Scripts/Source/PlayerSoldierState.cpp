#include "PlayerSoldierState.h"
#include "PlayerSoldier.h"
#include "Vitesse.h"
#include "Transform.h"

using namespace AbyssEngine;

void SoldierState::Move::Initialize()
{

}

void SoldierState::Move::Update()
{
    owner_->InputMove();

    owner_->BoardingTheVitesse(5.0f);
}

void SoldierState::Move::Finalize()
{

}
