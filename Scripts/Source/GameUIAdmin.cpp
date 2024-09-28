#include "GameUIAdmin.h"
#include "Actor.h"
#include "SpriteRenderer.h"
#include "PlayerSoldier.h"
#include "Engine.h"
#include "DxSystem.h"
#include "Vitesse.h"

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

    //spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/HUD_LockOn_Base.png"));
    spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/CenterRing_Out.png"));
    //spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Base)]->SetSize();
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Base)]->SetOffsetPosition(center);
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Base)]->SetPivot({0.5f,0.5f});
    //spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Base)]->SetScale({0.5f,0.5f});
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Base)]->SetColorAlpha(0.5f);


    //spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/HUD_LockOn_Circle.png"));
    //spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Circle)]->SetOffsetPosition(center);
    //spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Circle)]->SetPivot({0.5f,0.5f});
    //spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Circle)]->SetScale({0.5f,0.5f});

    spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Reticle.png"));
    spriteList_[static_cast<int>(Usefulness::Vitesse_Reticle)]->SetOffsetPosition(center);
    spriteList_[static_cast<int>(Usefulness::Vitesse_Reticle)]->SetPivot({ 0.5f,0.5f });
    //spriteList_[static_cast<int>(Usefulness::Vitesse_Reticle)]->SetScale({ 0.5f,0.5f });

    spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Lockon_Out.png"));
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_Out)]->SetOffsetPosition(center);
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_Out)]->SetPivot({ 0.5f,0.5f });
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_Out)]->SetScale({ 0.7f,0.7f });
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_Out)]->SetColorAlpha(0.7f);

    spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Lockon_In.png"));
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_In)]->SetOffsetPosition(center);
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_In)]->SetPivot({ 0.5f,0.5f });
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_In)]->SetScale({ 0.5f,0.5f });
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_In)]->SetColorAlpha(0.7f);



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
        //spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Circle)]->SetEnable(true);
        spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_In)]->SetEnable(true);
        spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_Out)]->SetEnable(true);
        spriteList_[static_cast<int>(Usefulness::Vitesse_Reticle)]->SetEnable(true);

        //通常のレティクルを消しておく
        spriteList_[static_cast<int>(Usefulness::Reticle)]->SetEnable(false);

        //ロックオンしているときのみレティクルを変更
        const auto& vitesse = player_->GetMyVitesse();
        if (vitesse->GetActiveLockon())
        {
            //ロックオン時
            spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_In)]->SetEnable(true);
            spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_Out)]->SetEnable(true);

            spriteList_[static_cast<int>(Usefulness::Vitesse_Reticle)]->SetEnable(false);
        }
        else
        {
            //非ロックオン時
            spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_In)]->SetEnable(false);
            spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_Out)]->SetEnable(false);

            spriteList_[static_cast<int>(Usefulness::Vitesse_Reticle)]->SetEnable(true);
        }
    }
    else
    {
        spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Base)]->SetEnable(false);
        //spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Circle)]->SetEnable(false);
        spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_In)]->SetEnable(false);
        spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_Out)]->SetEnable(false);
        spriteList_[static_cast<int>(Usefulness::Vitesse_Reticle)]->SetEnable(false);

        //通常のレティクルをアクティブに
        spriteList_[static_cast<int>(Usefulness::Reticle)]->SetEnable(true);
    }

    

    //回転させる
    //float angle = spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Circle)]->GetAngle();
    //spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Circle)]->SetAngle(angle + 15.0f * Time::deltaTime_);
    float angle = spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_In)]->GetAngle();
    spriteList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_In)]->SetAngle(angle + 15.0f * Time::deltaTime_);
}
