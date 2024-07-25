#include "GameUIAdmin.h"
#include "Actor.h"
#include "SpriteRenderer.h"
#include "PlayerSoldier.h"

using namespace AbyssEngine;

void GameUIAdmin::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    ScriptComponent::Initialize(actor);

    //各種スプライトを登録
    spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/UI_Boarding.png"));
    spriteList_[static_cast<int>(Usefulness::Boarding)]->SetColorAlpha(0.0f);

    spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/Reticle.png"));
    spriteList_[static_cast<int>(Usefulness::Reticle)]->SetSize({16.0f,16.0f});
    spriteList_[static_cast<int>(Usefulness::Reticle)]->SetOffsetPosition({954.0f,524.0f});
}

void GameUIAdmin::UpdateAfter()
{
    if (player_->GetCanBoarding())
    {
        spriteList_[static_cast<int>(Usefulness::Boarding)]->FadeIn(1.0f, 3.0f);
    }
    else
    {
        spriteList_[static_cast<int>(Usefulness::Boarding)]->FadeOut(0.0f, 10.0f);
    }
}
