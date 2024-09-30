#include "GameUIAdmin.h"
#include "Actor.h"
#include "SpriteRenderer.h"
#include "PlayerSoldier.h"
#include "Engine.h"
#include "DxSystem.h"
#include "Vitesse.h"

using namespace AbyssEngine;

Vector2 GameUIAdmin::screenCenter_ = {};

void GameUIAdmin::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    D3D11_VIEWPORT viewport;
    DXSystem::GetViewport(1, &viewport);

    ScriptComponent::Initialize(actor);

    screenCenter_ = { viewport.Width / 2, viewport.Height / 2 };

    //�e��X�v���C�g��o�^
    spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/UI_Boarding.png"));
    GetUI(Usefulness::Boarding)->SetColorAlpha(0.0f);

    spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/Reticle.png"));
    //GetUI(Usefulness::Reticle)->SetSize({16.0f,16.0f});
    //GetUI(Usefulness::Reticle)->SetOffsetPosition({940.0f,513.0f});
    GetUI(Usefulness::Reticle)->SetOffsetPosition({ screenCenter_ });
    GetUI(Usefulness::Reticle)->SetPivot({0.5f,0.5f});
    GetUI(Usefulness::Reticle)->SetScale({0.5f,0.5f});

    //spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/HUD_LockOn_Base.png"));
    spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/CenterRing_Out.png"));
    //GetUI(Usefulness::HUD_Reticle_Base)->SetSize();
    GetUI(Usefulness::HUD_Reticle_Base)->SetOffsetPosition(screenCenter_);
    GetUI(Usefulness::HUD_Reticle_Base)->SetPivot({0.5f,0.5f});
    //GetUI(Usefulness::HUD_Reticle_Base)->SetScale({0.5f,0.5f});
    GetUI(Usefulness::HUD_Reticle_Base)->SetColorAlpha(0.5f);


    //spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/HUD_LockOn_Circle.png"));
    //GetUI(Usefulness::HUD_Reticle_Circle)->SetOffsetPosition(screenCenter_);
    //GetUI(Usefulness::HUD_Reticle_Circle)->SetPivot({0.5f,0.5f});
    //GetUI(Usefulness::HUD_Reticle_Circle)->SetScale({0.5f,0.5f});

    spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Reticle.png"));
    GetUI(Usefulness::Vitesse_Reticle)->SetOffsetPosition(screenCenter_);
    GetUI(Usefulness::Vitesse_Reticle)->SetPivot({ 0.5f,0.5f });
    //GetUI(Usefulness::Vitesse_Reticle)->SetScale({ 0.5f,0.5f });

    spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Lockon_Out.png"));
    GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetOffsetPosition(screenCenter_);
    GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetPivot({ 0.5f,0.5f });
    GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetScale({ 0.7f,0.7f });
    //GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetColorAlpha(0.7f);
    GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetColor(Vector4(1,0.6f,0,0.4f));

    spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Lockon_In.png"));
    GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetOffsetPosition(screenCenter_);
    GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetPivot({ 0.5f,0.5f });
    GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetScale({ 0.5f,0.5f });
    //GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetColorAlpha(0.7f);
    GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetColor(Vector4(1,0,0,0.9f));
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
        GetUI(Usefulness::Boarding)->FadeIn(1.0f, 3.0f);
    }
    else
    {
        GetUI(Usefulness::Boarding)->FadeOut(0.0f, 10.0f);
    }

    //���B�e�X���掞�̓��e�B�N����UI������
    if (player_->GetVitesseOnBoard())
    {
        GetUI(Usefulness::Reticle)->SetEnable(false);
    }
    else
    {
        GetUI(Usefulness::Reticle)->SetEnable(true);
    }
}

void GameUIAdmin::UiUpdateVitesse()
{
    //���B�e�X���掞�̓��e�B�N����UI������
    if (player_->GetVitesseOnBoard())
    {
        GetUI(Usefulness::HUD_Reticle_Base)->SetEnable(true);
        //GetUI(Usefulness::HUD_Reticle_Circle)->SetEnable(true);
        GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetEnable(true);
        GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetEnable(true);
        GetUI(Usefulness::Vitesse_Reticle)->SetEnable(true);

        //�ʏ�̃��e�B�N���������Ă���
        GetUI(Usefulness::Reticle)->SetEnable(false);

        //���b�N�I�����Ă���Ƃ��̂݃��e�B�N����ύX
        const auto& vitesse = player_->GetMyVitesse();
        //���b�N�I������Ă���A�N�^�[�擾
        const auto& lockonActor = vitesse->GetLockonTarget().lock();
        if (lockonActor)
        {
            //���b�N�I����
            GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetEnable(true);
            GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetEnable(true);

            GetUI(Usefulness::Vitesse_Reticle)->SetEnable(false);

            //���W�ړ�
            LockonUiMove();
        }
        else
        {
            //�񃍃b�N�I����
            GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetEnable(false);
            GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetEnable(false);

            GetUI(Usefulness::Vitesse_Reticle)->SetEnable(true);

            //���W�ړ�
            GetUI(Usefulness::HUD_Reticle_Base)->SetOffsetPosition(screenCenter_);
        }
    }
    else
    {
        GetUI(Usefulness::HUD_Reticle_Base)->SetEnable(false);
        //GetUI(Usefulness::HUD_Reticle_Circle)->SetEnable(false);
        GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetEnable(false);
        GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetEnable(false);
        GetUI(Usefulness::Vitesse_Reticle)->SetEnable(false);

        //�ʏ�̃��e�B�N�����A�N�e�B�u��
        GetUI(Usefulness::Reticle)->SetEnable(true);
    }

    

    //��]������
    //float angle = GetUI(Usefulness::HUD_Reticle_Circle)->GetAngle();
    //GetUI(Usefulness::HUD_Reticle_Circle)->SetAngle(angle + 15.0f * Time::deltaTime_);
    float angle = GetUI(Usefulness::HUD_Reticle_Lockon_In)->GetAngle();
    GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetAngle(angle + 15.0f * Time::deltaTime_);
}

void GameUIAdmin::LockonUiMove()
{
    //���b�N�I�����Ă���Ƃ��̂݃��e�B�N����ύX
    const auto& vitesse = player_->GetMyVitesse();
    //���b�N�I������Ă���A�N�^�[�擾
    const auto& lockonActor = vitesse->GetLockonTarget().lock();

    //���b�N�ύX����������
    if (vitesse->GetChangeLockonTarget())
    {
        //����Ȃ�^�C�}�[�̏�����
        lockonMovetimer_ = 0;
    }

    //���W�ړ�
    const Vector2 curScreenPos = GetUI(Usefulness::HUD_Reticle_Base)->GetOffsetPosition();
    const Vector2 targetScreenPos = vitesse->GetCamera()->WorldToScreenPosition(lockonActor->GetTransform()->GetPosition());
    Vector2 screenPos;

    //���b�N�ύX���UI�ړ����Ԃ�
    if (lockonMovetimer_ < Lockon_Move_Time)
    {
        //�⊮�l
        float weight = lockonMovetimer_ / Lockon_Move_Time;

        //�ʒu��⊮������
        screenPos = Vector2::Lerp(curScreenPos, targetScreenPos, weight);
    }
    else
    {
        screenPos = targetScreenPos;
    }
    

    GetUI(Usefulness::HUD_Reticle_Base)->SetOffsetPosition(screenPos);
    GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetOffsetPosition(screenPos);
    GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetOffsetPosition(screenPos);

    //�^�C�}�[�X�V
    lockonMovetimer_ += Time::deltaTime_;
}
