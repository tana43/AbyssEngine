#include "GameUIAdmin.h"
#include "Actor.h"
#include "SpriteRenderer.h"
#include "PlayerSoldier.h"
#include "Engine.h"
#include "DxSystem.h"

using namespace AbyssEngine;

void GameUIAdmin::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    D3D11_VIEWPORT viewport;
    DXSystem::GetViewport(1, &viewport);

    ScriptComponent::Initialize(actor);

    Vector2 center = { viewport.Width / 2, viewport.Height / 2 };

    //各種スプライトを登録
    spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/UI_Boarding.png"));
    spriteList_[static_cast<int>(Usefulness::Boarding)]->SetColorAlpha(0.0f);

    spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/Reticle.png"));
    //spriteList_[static_cast<int>(Usefulness::Reticle)]->SetSize({16.0f,16.0f});
    //spriteList_[static_cast<int>(Usefulness::Reticle)]->SetOffsetPosition({940.0f,513.0f});
    spriteList_[static_cast<int>(Usefulness::Reticle)]->SetOffsetPosition({ center });
    spriteList_[static_cast<int>(Usefulness::Reticle)]->SetPivot({0.5f,0.5f});
    spriteList_[static_cast<int>(Usefulness::Reticle)]->SetScale({0.5f,0.5f});

    spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/HUD_LockOn_Base.png"));
    //spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Base)]->SetSize();
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Base)]->SetOffsetPosition(center);
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Base)]->SetPivot({0.5f,0.5f});
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Base)]->SetScale({0.5f,0.5f});


    spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/HUD_LockOn_Circle.png"));
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Circle)]->SetOffsetPosition(center);
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Circle)]->SetPivot({0.5f,0.5f});
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Circle)]->SetScale({0.5f,0.5f});

}

void GameUIAdmin::UpdateAfter()
{
    UiUpdatePlayerSoldier();

    UiUpdateVitesse();
}

void GameUIAdmin::UiUpdatePlayerSoldier()
{
    if (player_->GetCanBoarding())
    {
        spriteList_[static_cast<int>(Usefulness::Boarding)]->FadeIn(1.0f, 3.0f);
    }
    else
    {
        spriteList_[static_cast<int>(Usefulness::Boarding)]->FadeOut(0.0f, 10.0f);
    }

    //ヴィテス搭乗時はレティクルのUIを消す
    if (player_->GetVitesseOnBoard())
    {
        spriteList_[static_cast<int>(Usefulness::Reticle)]->SetEnable(false);
    }
    else
    {
        spriteList_[static_cast<int>(Usefulness::Reticle)]->SetEnable(true);
    }
}

void GameUIAdmin::UiUpdateVitesse()
{
    //ヴィテス搭乗時はレティクルのUIを消す
    if (player_->GetVitesseOnBoard())
    {
        spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Base)]->SetEnable(true);
        spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Circle)]->SetEnable(true);
    }
    else
    {
        spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Base)]->SetEnable(false);
        spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Circle)]->SetEnable(false);
    }

    //回転させる
    float angle = spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Circle)]->GetAngle();
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Circle)]->SetAngle(angle + 15.0f * Time::deltaTime_);
}
