#include "GameUIAdmin.h"
#include "Actor.h"
#include "SpriteRenderer.h"
#include "PlayerSoldier.h"
#include "Engine.h"
#include "DxSystem.h"
#include "Vitesse.h"
#include "BossMech.h"
#include "AudioSource.h"

#include "Input.h"

#include "Easing.h"

using namespace AbyssEngine;

Vector2 GameUIAdmin::screenCenter_ = {};

void GameUIAdmin::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    D3D11_VIEWPORT viewport;
    DXSystem::GetViewport(1, &viewport);

    ScriptComponent::Initialize(actor);

    screenCenter_ = { viewport.Width / 2, viewport.Height / 2 };

    //ロード
    LoadSprite();

    //各種スプライトを登録
    GetUI(Usefulness::Boarding)->SetColorAlpha(0.0f);

    //GetUI(Usefulness::Reticle)->SetSize({16.0f,16.0f});
    //GetUI(Usefulness::Reticle)->SetOffsetPosition({940.0f,513.0f});
    GetUI(Usefulness::Reticle)->SetOffsetPosition({ screenCenter_ });
    GetUI(Usefulness::Reticle)->SetPivot({0.5f,0.5f});
    GetUI(Usefulness::Reticle)->SetScale({0.25f,0.25f});

    GetUI(Usefulness::Bullet_Hit)->SetOffsetPosition({ screenCenter_ });
    GetUI(Usefulness::Bullet_Hit)->SetPivot({ 0.5f,0.5f });
    GetUI(Usefulness::Bullet_Hit)->SetScale({0.7f,0.7f});

    //spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/HUD_LockOn_Base.png"));
    //GetUI(Usefulness::HUD_Reticle_Base)->SetSize();
    GetUI(Usefulness::HUD_Reticle_Base)->SetOffsetPosition(screenCenter_);
    GetUI(Usefulness::HUD_Reticle_Base)->SetPivot({0.5f,0.5f});
    //GetUI(Usefulness::HUD_Reticle_Base)->SetScale({0.5f,0.5f});
    GetUI(Usefulness::HUD_Reticle_Base)->SetColorAlpha(0.5f);


    //spriteList_.emplace_back(actor->AddComponent<SpriteRenderer>("./Assets/Images/HUD_LockOn_Circle.png"));
    //GetUI(Usefulness::HUD_Reticle_Circle)->SetOffsetPosition(screenCenter_);
    //GetUI(Usefulness::HUD_Reticle_Circle)->SetPivot({0.5f,0.5f});
    //GetUI(Usefulness::HUD_Reticle_Circle)->SetScale({0.5f,0.5f});

    GetUI(Usefulness::Vitesse_Reticle)->SetOffsetPosition(screenCenter_);
    GetUI(Usefulness::Vitesse_Reticle)->SetPivot({ 0.5f,0.5f });
    //GetUI(Usefulness::Vitesse_Reticle)->SetScale({ 0.5f,0.5f });

    GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetOffsetPosition(screenCenter_);
    GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetPivot({ 0.5f,0.5f });
    GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetScale({ 0.7f,0.7f });
    //GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetColorAlpha(0.7f);
    GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetColor(Vector4(1,0.6f,0,0.4f));

    GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetOffsetPosition(screenCenter_);
    GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetPivot({ 0.5f,0.5f });
    GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetScale({ 0.5f,0.5f });
    //GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetColorAlpha(0.7f);
    GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetColor(Vector4(1,0,0,0.9f));

    GetUI(Usefulness::HUD_Boost_Gauge_Back)->SetOffsetPosition(Vector2(600.0f,941.0f));
    GetUI(Usefulness::HUD_Boost_Gauge_Main)->SetOffsetPosition(Vector2(605.2f,941.0f));
    GetUI(Usefulness::HUD_Boost_Gauge_Main)->SetSize(Vector2(678.3f,11.0f));


    //敵のHP表示
    GetUI(Usefulness::Boss_HP_Gauge_Back)->SetOffsetPosition(Vector2(688.3f, 16.9f));
    GetUI(Usefulness::Boss_HP_Gauge_Main)->SetOffsetPosition(Vector2(691.3f, 16.9f));
    GetUI(Usefulness::Boss_HP_Gauge_Main)->SetSize(Vector2(539.0f, 16.0f));
    GetUI(Usefulness::Boss_HP_Gauge_Dmg)->SetOffsetPosition(Vector2(691.3f, 16.9f));
    GetUI(Usefulness::Boss_HP_Gauge_Dmg)->SetSize(Vector2(539.0f, 16.0f));
    GetUI(Usefulness::Boss_HP_Gauge_Dmg)->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));

    GetUI(Usefulness::Boss_Name)->SetOffsetPosition(Vector2(874.8f, 35.4f));
    GetUI(Usefulness::Boss_Name)->SetScale(Vector2(0.4f, 0.4f));


    //自機のHP表示
    GetUI(Usefulness::HUD_HP_Gauge_Back)->SetOffsetPosition(Vector2(44.400, 864.8f));
    GetUI(Usefulness::HUD_HP_Gauge_Main)->SetOffsetPosition(Vector2(49.2f, 864.3f));
    GetUI(Usefulness::HUD_HP_Gauge_Dmg)->SetOffsetPosition(Vector2(49.2f, 864.3f));
    GetUI(Usefulness::HUD_HP_Gauge_Back)->SetSize(Vector2(457.7f, 13.8f));
    GetUI(Usefulness::HUD_HP_Gauge_Main)->SetSize(Vector2(448.9f, 14.1f));
    GetUI(Usefulness::HUD_HP_Gauge_Dmg)->SetSize(Vector2(448.9f, 14.1f));
    GetUI(Usefulness::HUD_HP_Gauge_Dmg)->SetColor(Vector4(1.0f,0.0f,0.0f,1.0f));

    GetUI(Usefulness::HUD_Vitesse_Name)->SetOffsetPosition(Vector2(48.3f, 848.2f));
    GetUI(Usefulness::HUD_Vitesse_Name)->SetScale(Vector2(0.4f, 0.4f));
    
    GetUI(Usefulness::HUD_Decoration)->SetColorAlpha(0.2f);
    GetUI(Usefulness::HUD_Decoration)->SetOffsetPosition(screenCenter_);
    GetUI(Usefulness::HUD_Decoration)->SetPivot({ 0.5f,0.5f });

    //システム起動演出で使用するUIを登録
    systemStartUpUIs_.emplace_back(Usefulness::HUD_Decoration);
    systemStartUpUIs_.emplace_back(Usefulness::HUD_Reticle_Lockon_In);
    systemStartUpUIs_.emplace_back(Usefulness::HUD_Reticle_Lockon_Out);
    systemStartUpUIs_.emplace_back(Usefulness::HUD_Reticle_Base);

    //エラー表示UI設定
    for (int i = static_cast<int>(Usefulness::HUD_Error_Acoustic_Censor); i <= static_cast<int>(Usefulness::HUD_Error_Weapon);i++)
    {
        spriteList_[i]->SetPivot(Vector2(0.5f, 0.5f));
        spriteList_[i]->SetColorAlpha(0.5f);
    }
    GetUI(Usefulness::HUD_Error_Acoustic_Censor)->SetOffsetPosition(Vector2(540.0f, 200.0f));
    GetUI(Usefulness::HUD_Error_Missing_Parts)->SetOffsetPosition(Vector2(350.0f, 450.0f));
    GetUI(Usefulness::HUD_Error_Movement)->SetOffsetPosition(Vector2(500.0f, 700.0f));
    GetUI(Usefulness::HUD_Error_Overheat)->SetOffsetPosition(Vector2(1380.0f, 200.0f));
    GetUI(Usefulness::HUD_Error_Stagger)->SetOffsetPosition(Vector2(1570.0f, 450.0f));
    GetUI(Usefulness::HUD_Error_Weapon)->SetOffsetPosition(Vector2(1420.0f, 700.0f));


    //起動演出関係
    {
        GetUI(Usefulness::System_Start_Up)->SetPivot(Vector2(0.5f, 0.5f));
        GetUI(Usefulness::System_Start_Up)->SetOffsetPosition(screenCenter_);

        GetUI(Usefulness::SSU_Frame)->SetOffsetPosition(Vector2(1190.0f, 198.0f));
        GetUI(Usefulness::SSU_Frame)->SetColorAlpha(0.5f);

        GetUI(Usefulness::SSU_Back)->SetEnable(false);

        const int lineSpacingY = 96.0f;//行間

        int loopCount = 0;
        for (int i = static_cast<int>(Usefulness::SSU_Energy_Output); i <= static_cast<int>(Usefulness::SSU_Alert_System); i++)
        {
            spriteList_[i]->SetOffsetPosition(Vector2(
            1210.0f,220.0f + lineSpacingY * loopCount
            ));
            spriteList_[i]->SetColorAlpha(0.5f);
            loopCount++;
        }

        loopCount = 0;
        for (int i = static_cast<int>(Usefulness::SSU_Energy_Output_Gauge); i <= static_cast<int>(Usefulness::SSU_Alert_System_Gauge); i++)
        {
            spriteList_[i]->SetOffsetPosition(Vector2(
                1217.2f, 224.9f + lineSpacingY * loopCount
            ));
            spriteList_[i]->SetColorAlpha(0.5f);
            loopCount++;
        }


        //起動演出に使用するUIの登録
        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::SSU_Back).get());
        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::SSU_Energy_Output).get());
        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::SSU_Drive_System).get());
        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::SSU_Weapon_System).get());
        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::SSU_Boost_System).get());
        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::SSU_Alert_System).get());
        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::SSU_Energy_Output_Gauge).get());
        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::SSU_Drive_System_Gauge).get());
        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::SSU_Weapon_System_Gauge).get());
        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::SSU_Boost_System_Gauge).get());
        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::SSU_Alert_System_Gauge).get());
        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::SSU_Frame).get());
        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::System_Start_Up).get());

        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::HUD_Error_Acoustic_Censor ).get());
        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::HUD_Error_Missing_Parts).get());
        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::HUD_Error_Movement).get());
        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::HUD_Error_Overheat).get());
        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::HUD_Error_Stagger).get());
        sSUUpdateSprites_.emplace_back(GetUI(Usefulness::HUD_Error_Weapon).get());

        //起動演出後に表示するUIの登録
        sSUCompleteLaunchSprites_.emplace_back(GetUI(Usefulness::HUD_Reticle_Base).get());
        //sSUCompleteLaunchSprites_.emplace_back(GetUI(Usefulness::HUD_Reticle_Lockon_In).get());
        //sSUCompleteLaunchSprites_.emplace_back(GetUI(Usefulness::HUD_Reticle_Lockon_Out).get());
        sSUCompleteLaunchSprites_.emplace_back(GetUI(Usefulness::Vitesse_Reticle).get());

        sSUCompleteLaunchSprites_.emplace_back(GetUI(Usefulness::HUD_Boost_Gauge_Back).get());
        sSUCompleteLaunchSprites_.emplace_back(GetUI(Usefulness::HUD_Boost_Gauge_Main).get());

        sSUCompleteLaunchSprites_.emplace_back(GetUI(Usefulness::HUD_Decoration).get());

        sSUCompleteLaunchSprites_.emplace_back(GetUI(Usefulness::Boss_HP_Gauge_Back).get());
        sSUCompleteLaunchSprites_.emplace_back(GetUI(Usefulness::Boss_HP_Gauge_Dmg).get());
        sSUCompleteLaunchSprites_.emplace_back(GetUI(Usefulness::Boss_HP_Gauge_Main).get());
        sSUCompleteLaunchSprites_.emplace_back(GetUI(Usefulness::Boss_Name).get());
        sSUCompleteLaunchSprites_.emplace_back(GetUI(Usefulness::HUD_HP_Gauge_Back).get());
        sSUCompleteLaunchSprites_.emplace_back(GetUI(Usefulness::HUD_HP_Gauge_Dmg).get());
        sSUCompleteLaunchSprites_.emplace_back(GetUI(Usefulness::HUD_HP_Gauge_Main).get());
        sSUCompleteLaunchSprites_.emplace_back(GetUI(Usefulness::HUD_Vitesse_Name).get());
    }

    //起動演出に必要なUIを非表示
    for (auto& s : sSUUpdateSprites_)
    {
        s->SetEnable(false);
    }

    for (int i = 0;i < spriteList_.size();i++)
    {
        initColorAlphaList_.emplace_back(spriteList_[i]->GetColorAlpha());
    }

    //今のHPを登録
    if (const auto& p = player_.lock())
    {
        preVitesseHp_ = p->GetMyVitesse()->GetHealth();
    }
    if (const auto& b = boss_.lock())
    {
        preBossHp_ = b->GetHealth();
    }

    //サウンド初期化
    AudioInitialize();

    //歩兵時のUIに
    ToSoldierUI();
}

void GameUIAdmin::DrawImGui()
{
    if (ImGui::TreeNode("Game UI Admin"))
    {
        ImGui::DragFloat("Error Anim Speed", &SSUAnimationSpeed_, 0.01f);
       
        ImGui::Checkbox("System Start Up", &isSystemStartUp_);

        ImGui::TreePop();
    }

    
}

void GameUIAdmin::UpdateAfter()
{
    UiUpdatePlayerSoldier();

    UiUpdateVitesse();

    UiUpdateVitesseHpGauge();

    UiUpdateBoostGauge();

    UiUpdateBossHpGauge();

    UpdateSSUWindow();

    UpdateBulletHit();

    //起動演出
    UpdateSSU();

#if _DEBUG
    if (Keyboard::GetKeyDown(DirectX::Keyboard::D0))
    {
        isSystemStartUp_ = true;
    }
#endif

#if 0 //サムネ用
    GetUI(Usefulness::HUD_HP_Gauge_Back)->SetEnable(false);
    GetUI(Usefulness::HUD_HP_Gauge_Dmg)->SetEnable(false);
    GetUI(Usefulness::HUD_HP_Gauge_Main)->SetEnable(false);
    GetUI(Usefulness::HUD_Vitesse_Name)->SetEnable(false);
    GetUI(Usefulness::Boss_HP_Gauge_Back)->SetEnable(false);
    GetUI(Usefulness::Boss_HP_Gauge_Dmg)->SetEnable(false);
    GetUI(Usefulness::Boss_HP_Gauge_Main)->SetEnable(false);
    GetUI(Usefulness::HUD_Boost_Gauge_Back)->SetEnable(false);
    GetUI(Usefulness::HUD_Boost_Gauge_Main)->SetEnable(false);
    GetUI(Usefulness::Boss_Name)->SetEnable(false);
#endif // 1 //サムネ用


}

void GameUIAdmin::UiUpdatePlayerSoldier()
{
    if (const auto& p = player_.lock())
    {
        if (p->GetCanBoarding() && !p->GetVitesseOnBoard())
        {
            GetUI(Usefulness::Boarding)->FadeIn(1.0f, 3.0f);
        }
        else
        {
            GetUI(Usefulness::Boarding)->FadeOut(0.0f, 10.0f);
        }
    }

    //ヴィテス搭乗時はレティクルのUIを消す
    if (player_.lock()->GetVitesseOnBoard())
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
    if (isBoardVitesse_)
    {
        if (!player_.lock()->GetVitesseOnBoard())
        {
            //UIを切り替え
            ToSoldierUI();

            isBoardVitesse_ = false;
        }

            //システム起動していないか
            if (!isSystemStartUp_)
            {

            //ロックオンしているときのみレティクルを変更
            const auto& vitesse = player_.lock()->GetMyVitesse();
            //ロックオンされているアクター取得
            const auto& lockonActor = vitesse->GetLockonTarget().lock();
            if (lockonActor)
            {
                //ロックオン時
                GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetEnable(true);
                GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetEnable(true);

                GetUI(Usefulness::Vitesse_Reticle)->SetEnable(false);

                //座標移動
                LockonUiMove();
            }
            else
            {
                //非ロックオン時
                GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetEnable(false);
                GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetEnable(false);

                GetUI(Usefulness::Vitesse_Reticle)->SetEnable(true);

                //座標移動
                GetUI(Usefulness::HUD_Reticle_Base)->SetOffsetPosition(screenCenter_);
            }
        }
    }
    else
    {
        if (player_.lock()->GetVitesseOnBoard())
        {
            //UIを切り替え
            ToVitessseUI();

            isBoardVitesse_ = true;
        }
    }

    

    //回転させる
    //float angle = GetUI(Usefulness::HUD_Reticle_Circle)->GetAngle();
    //GetUI(Usefulness::HUD_Reticle_Circle)->SetAngle(angle + 15.0f * actor_->GetDeltaTime());
    float angle = GetUI(Usefulness::HUD_Reticle_Lockon_In)->GetAngle();
    GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetAngle(angle + 15.0f * actor_->GetDeltaTime());
}

void GameUIAdmin::UiUpdateBoostGauge()
{
    //ブーストゲージを更新

    auto& gauge = GetUI(Usefulness::HUD_Boost_Gauge_Main);

    if (const auto& vitesse = player_.lock()->GetMyVitesse())
    {
        float max = vitesse->GetMaxBoostAmount();
        float now = vitesse->GetBoostAmount();

        gauge->SetScale(Vector2(
            std::clamp(now / max, 0.0f, 1.0f),
            1.0f
        ));

        //オーバーヒートしているなら点滅させる
        if (vitesse->GetIsBoostOverHeat())
        {
            gauge->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));

            gauge->SetColorAlpha(fabsf(sinf(boostGaugeBlinkTimer_)));

            boostGaugeBlinkTimer_ += Time::GetDeltaTime() * 12.0f;
        }
        else
        {
            //オーバーヒートしていないなら白に戻す
            gauge->SetColor(Vector4::One);
        }
    }
}

void GameUIAdmin::UiUpdateVitesseHpGauge()
{
    if (const auto& v = player_.lock()->GetMyVitesse())
    {
        auto& gauge = GetUI(Usefulness::HUD_HP_Gauge_Main);
        float max = v->GetMaxHealth();
        float now = v->GetHealth();

        gauge->SetScale(Vector2(
            std::clamp(now / max, 0.0f, 1.0f),
            1.0f
        ));

        //ダメージゲージを更新
        auto& dmgGauge = GetUI(Usefulness::HUD_HP_Gauge_Dmg);

        //ダメージを受けてからしばらく経っているか
        if (vitesseDamagedTimer_ > damageUiHoldTime)
        {
            //スケールを算出
            float scaleX = dmgGauge->GetScale().x;
            scaleX -= Time::GetDeltaTime();
            scaleX = max(scaleX, gauge->GetScale().x);
            dmgGauge->SetScaleX(scaleX);
        }

        //HPが前回と変わっていないか
        if (preVitesseHp_ != now)
        {
            //変わった
            preVitesseHp_ = now;

            //タイマーリセット
            vitesseDamagedTimer_ = 0.0f;
        }

        vitesseDamagedTimer_ += Time::GetDeltaTime();
    }
}

void GameUIAdmin::UiUpdateBossHpGauge()
{
    if (const auto& b = boss_.lock())
    {
        auto& gauge = GetUI(Usefulness::Boss_HP_Gauge_Main);
        float max = b->GetMaxHealth();
        float now = b->GetHealth();

        gauge->SetScale(Vector2(
            std::clamp(now / max, 0.0f, 1.0f),
            1.0f
        ));

        //ダメージゲージを更新
        auto& dmgGauge = GetUI(Usefulness::Boss_HP_Gauge_Dmg);

        //ダメージを受けてからしばらく経っているか
        if (bossDamagedTimer_ > damageUiHoldTime)
        {
            //スケールを算出
            float scaleX = dmgGauge->GetScale().x;
            scaleX -= Time::GetDeltaTime();
            scaleX = max(scaleX, gauge->GetScale().x);
            dmgGauge->SetScaleX(scaleX);
        }

        //HPが前回と変わっていないか
        if (preBossHp_ != now)
        {
            //変わった
            preBossHp_ = now;

            //タイマーリセット
            bossDamagedTimer_ = 0.0f;
        }

        bossDamagedTimer_ += Time::GetDeltaTime();
    }
    else
    {
        GetUI(Usefulness::Boss_Name)->SetEnable(false);
        GetUI(Usefulness::Boss_HP_Gauge_Back)->SetEnable(false);
        GetUI(Usefulness::Boss_HP_Gauge_Dmg)->SetEnable(false);
        GetUI(Usefulness::Boss_HP_Gauge_Main)->SetEnable(false);
    }
}

void GameUIAdmin::BulletHit()
{
    GetUI(Usefulness::Bullet_Hit)->SetColorAlpha(1.0f);
}

void GameUIAdmin::LockonUiMove()
{
    //ロックオンしているときのみレティクルを変更
    const auto& vitesse = player_.lock()->GetMyVitesse();
    //ロックオンされているアクター取得
    const auto& lockonActor = vitesse->GetLockonTarget().lock();

    //ロック変更があったか
    if (vitesse->GetChangeLockonTarget())
    {
        //あるならタイマーの初期化
        lockonMovetimer_ = 0;
    }

    //座標移動
    const Vector2 curScreenPos = GetUI(Usefulness::HUD_Reticle_Base)->GetOffsetPosition();
    const Vector2 targetScreenPos = vitesse->GetCamera()->WorldToScreenPosition(lockonActor->GetTransform()->GetPosition());
    Vector2 screenPos;

    //ロック変更後のUI移動時間か
    if (lockonMovetimer_ < Lockon_Move_Time)
    {
        //補完値
        float weight = lockonMovetimer_ / Lockon_Move_Time;

        //位置を補完させる
        screenPos = Vector2::Lerp(curScreenPos, targetScreenPos, weight);
    }
    else
    {
        screenPos = targetScreenPos;
    }
    

    GetUI(Usefulness::HUD_Reticle_Base)->SetOffsetPosition(screenPos);
    GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetOffsetPosition(screenPos);
    GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetOffsetPosition(screenPos);

    //タイマー更新
    lockonMovetimer_ += actor_->GetDeltaTime();
}

void GameUIAdmin::LoadSprite()
{
    //「搭乗する」
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI_Boarding.png"));

    //歩兵時レティクル
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Soldier/Reticle.png"));
    //ダメージを与えた際に表示するUI
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Soldier/Bullet_Hit.png"));

    //Vitesse HUD
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/CenterRing_Out.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Reticle.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Lockon_Out.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Lockon_In.png"));

    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/HUD_Decoration.png"));

    //ブーストゲージ
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Gauge_Back.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Gauge_Main.png"));

    //ボスHPゲージ
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Boss/Hp_Gauge_Back.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Boss/Hp_Gauge_Dmg.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Boss/Hp_Gauge_Main.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Boss/Text_Name.png"));

    //Vitesse HP
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Gauge_Back.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Gauge_Dmg.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Gauge_Main.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Text_Name.png"));

    //起動演出
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/StartUp/Back.png"));

    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/StartUp/Energy_Output.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/StartUp/Drive_System.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/StartUp/Weapon_System.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/StartUp/Boost_System.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/StartUp/Alert_System.png"));

    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/StartUp/Gauge.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/StartUp/Gauge.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/StartUp/Gauge.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/StartUp/Gauge.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/StartUp/Gauge.png"));

    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/StartUp/Frame.png"));

    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/StartUp/System_Start_Up.png"));


    //Errorリスト
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Error/Error_AcousticCensor.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Error/Error_MissingParts.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Error/Error_Movement.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Error/Error_Overheat.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Error/Error_Stagger.png"));
    spriteList_.emplace_back(actor_->AddComponent<SpriteRenderer>("./Assets/Images/UI/Vitesse/Error/Error_Weapon.png"));

    

    
}

void GameUIAdmin::LaunchSSUWindow(Usefulness ui)
{
    const int firstSSUSpriteIndex = static_cast<int>(Usefulness::SSU_Energy_Output);
    const int i = static_cast<int>(ui)- firstSSUSpriteIndex;
    errorActive_[i] = true;

    piSound_->Play();
}

void GameUIAdmin::CloseSSUWindow(Usefulness ui)
{
    const int firstSSUSpriteIndex = static_cast<int>(Usefulness::SSU_Energy_Output);
    const int i = static_cast<int>(ui) - firstSSUSpriteIndex;
    errorActive_[i] = false;

    piSound_->Play();
}

void GameUIAdmin::AllStopSSUWindow()
{
    for (auto& e : errorActive_)
    {
        e = false;
    }
}

void GameUIAdmin::UpdateSSUWindow()
{
    float animSpeed = Time::GetDeltaTime() * SSUAnimationSpeed_;
    const int firstSSUSpriteIndex = static_cast<int>(Usefulness::SSU_Energy_Output);

    for (int i = 0 ; i < Max_SSU_Ui;i++)
    {
        const auto& ui = spriteList_[firstSSUSpriteIndex + i];

        if (errorActive_[i])
        {
            errorTimer_[i] += animSpeed;
            if (errorTimer_[i] > 1.0f) errorTimer_[i] = 1.0f;
        }
        else
        {
            errorTimer_[i] -= animSpeed;
            if (errorTimer_[i] < 0.0f) errorTimer_[i] = 0.0f;
        }

        //アルファ変更
        //const float alpha = std::lerp(0.0f,errorColorAlpha_,errorTimer_[i]);
        //ui->SetColorAlpha(alpha);

        //スケールY変更
        const float scaleY = Easing::InCubic(errorTimer_[i], 1.0f, 1.0f, 0.0f);
        ui->SetScaleY(scaleY);
    }


#if _DEBUG
    if (Keyboard::GetKeyState().LeftAlt)
    {
        if (Keyboard::GetKeyDown(DirectX::Keyboard::T))errorActive_[0] = !errorActive_[0];
        if (Keyboard::GetKeyDown(DirectX::Keyboard::Y))errorActive_[1] = !errorActive_[1];
        if (Keyboard::GetKeyDown(DirectX::Keyboard::U))errorActive_[2] = !errorActive_[2];
        if (Keyboard::GetKeyDown(DirectX::Keyboard::I))errorActive_[3] = !errorActive_[3];
        if (Keyboard::GetKeyDown(DirectX::Keyboard::O))errorActive_[4] = !errorActive_[4];
        if (Keyboard::GetKeyDown(DirectX::Keyboard::P))errorActive_[5] = !errorActive_[5];
    }
#endif // _DEBUG

}

void GameUIAdmin::UpdateSSU()
{
    if (!isSystemStartUp_)return;

    switch (systemStartUpStep_)
    {
    case GameUIAdmin::SSUStep::Init:
    {


        //必要なUIの表示・非表示
        for (auto& s : sSUUpdateSprites_)
        {
            s->SetEnable(true);
        }

        for (auto& s : sSUCompleteLaunchSprites_)
        {
            s->SetEnable(false);
        }

        //背景の透過率を１に
        GetUI(Usefulness::SSU_Back)->SetColorAlpha(1.0f);

        //ゲージは０に
        for (int i = static_cast<int>(Usefulness::SSU_Energy_Output_Gauge); i <= static_cast<int>(Usefulness::SSU_Alert_System_Gauge); i++)
        {
            spriteList_[i]->SetScaleX(0.0f);
        }

        //SE再生
        sSUAlwaysSound_->Play();

        //次のステップへ
        systemStartUpStep_ = SSUStep::WindowLaunch;
        break;
    }
    case GameUIAdmin::SSUStep::WindowLaunch:
    {

        //システムロードのウィンドウ
        {

        LaunchSSUWindow(Usefulness::System_Start_Up);

            if (systemStartUpTimer_ > 0.015f)
            {
                LaunchSSUWindow(Usefulness::SSU_Energy_Output);
                LaunchSSUWindow(Usefulness::SSU_Energy_Output_Gauge);
            }

            if (systemStartUpTimer_ > 0.025f)
            {
                LaunchSSUWindow(Usefulness::SSU_Alert_System);
                LaunchSSUWindow(Usefulness::SSU_Alert_System_Gauge);
            }

            if (systemStartUpTimer_ > 0.035f)
            {
                LaunchSSUWindow(Usefulness::SSU_Weapon_System);
                LaunchSSUWindow(Usefulness::SSU_Weapon_System_Gauge);
            }

            if (systemStartUpTimer_ > 0.045f)
            {
                LaunchSSUWindow(Usefulness::SSU_Drive_System);
                LaunchSSUWindow(Usefulness::SSU_Drive_System_Gauge);
            }

            if (systemStartUpTimer_ > 0.055f)
            {
                LaunchSSUWindow(Usefulness::SSU_Boost_System);
                LaunchSSUWindow(Usefulness::SSU_Boost_System_Gauge);
            }
        }

        //エラー表示のウィンドウ
        {

            if (systemStartUpTimer_ > 0.015f)
            {
                LaunchSSUWindow(Usefulness::HUD_Error_Acoustic_Censor);
            }

            if (systemStartUpTimer_ > 0.012f)
            {
                LaunchSSUWindow(Usefulness::HUD_Error_Stagger);
            }

            if (systemStartUpTimer_ > 0.034f)
            {
                LaunchSSUWindow(Usefulness::HUD_Error_Weapon);
            }

            if (systemStartUpTimer_ > 0.055f)
            {
                LaunchSSUWindow(Usefulness::HUD_Error_Overheat);
            }

            if (systemStartUpTimer_ > 0.067f)
            {
                LaunchSSUWindow(Usefulness::HUD_Error_Missing_Parts);
            }

            if (systemStartUpTimer_ > 0.087f)
            {
                LaunchSSUWindow(Usefulness::HUD_Error_Movement);
            }
        }

        if (systemStartUpTimer_ > sSuLaunchTime_)
        {
            systemStartUpTimer_ = 0.0f;
            systemStartUpStep_ = SSUStep::Loading;
        }
        break;
    }
    case GameUIAdmin::SSUStep::Loading:
    {

        //それぞれのゲージが進み始めるタイミングをどの程度変えるか
        const float diffTime = 0.08f;

        //ゲージを進ませる
        int loopCount = 0;
        for (int i = static_cast<int>(Usefulness::SSU_Energy_Output_Gauge); i <= static_cast<int>(Usefulness::SSU_Alert_System_Gauge); i++)
        {
            float scaleX = Easing::InCirc(
                max(systemStartUpTimer_ - diffTime * loopCount,0.0f),
                sSuLoadingTime_, 1.0f, 0.0f);

            scaleX = std::clamp(scaleX, 0.0f, 1.0f);

            spriteList_[i]->SetScaleX(scaleX);

            loopCount++;
        }

        if (systemStartUpTimer_ > sSuLoadingTime_ + diffTime * loopCount)
        {
            //起動演出UIを非表示に
            AllStopSSUWindow();

            //必要なUIを透過した状態で起動する
            for (auto& s : sSUCompleteLaunchSprites_)
            {
                s->SetEnable(true);
                s->SetColorAlpha(0.0f);
            }

            systemStartUpTimer_ = 0.0f;
            systemStartUpStep_ = SSUStep::WindowClose;
        }

        break;
    }
    case GameUIAdmin::SSUStep::WindowClose:
    {
        CloseSSUWindow(Usefulness::System_Start_Up);

        if (systemStartUpTimer_ > 0.015f)
        {
            CloseSSUWindow(Usefulness::SSU_Energy_Output);
            CloseSSUWindow(Usefulness::SSU_Energy_Output_Gauge);
        }

        if (systemStartUpTimer_ > 0.025f)
        {
            CloseSSUWindow(Usefulness::SSU_Alert_System);
            CloseSSUWindow(Usefulness::SSU_Alert_System_Gauge);
        }

        if (systemStartUpTimer_ > 0.035f)
        {
            CloseSSUWindow(Usefulness::SSU_Weapon_System);
            CloseSSUWindow(Usefulness::SSU_Weapon_System_Gauge);
        }

        if (systemStartUpTimer_ > 0.045f)
        {
            CloseSSUWindow(Usefulness::SSU_Drive_System);
            CloseSSUWindow(Usefulness::SSU_Drive_System_Gauge);
        }

        if (systemStartUpTimer_ > 0.055f)
        {
            CloseSSUWindow(Usefulness::SSU_Boost_System);
            CloseSSUWindow(Usefulness::SSU_Boost_System_Gauge);
        }
    }

    //エラー表示のウィンドウ
    {

        if (systemStartUpTimer_ > 0.015f)
        {
            CloseSSUWindow(Usefulness::HUD_Error_Acoustic_Censor);
        }

        if (systemStartUpTimer_ > 0.012f)
        {
            CloseSSUWindow(Usefulness::HUD_Error_Stagger);
        }

        if (systemStartUpTimer_ > 0.034f)
        {
            CloseSSUWindow(Usefulness::HUD_Error_Weapon);
        }

        if (systemStartUpTimer_ > 0.055f)
        {
            CloseSSUWindow(Usefulness::HUD_Error_Overheat);
        }

        if (systemStartUpTimer_ > 0.067f)
        {
            CloseSSUWindow(Usefulness::HUD_Error_Missing_Parts);
        }

        if (systemStartUpTimer_ > 0.087f)
        {
            CloseSSUWindow(Usefulness::HUD_Error_Movement);
        }
    
        if (systemStartUpTimer_ > sSuCloseTime_)
        {
            systemStartUpTimer_ = 0.0f;

            //SE再生
            sSUCompleteSound_->Play();

            systemStartUpStep_ = SSUStep::Complete;
        }
    }
        break;
    case GameUIAdmin::SSUStep::Complete:
    {

        //一部のUIのスケールを大きい状態から小さくしていき、透過率を落としていく
        float value = Easing::InCirc(systemStartUpTimer_, sSuCompleteTime_, 1.0f, 0.0f);
        value = std::clamp(value, 0.0f, 1.0f);

        float scale = 1.0f + 3.0f * (1.0f - value);

        /*GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetScale(scale, scale);
        GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetColorAlpha(value * initColorAlphaList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_In)]);

        GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetScale(scale, scale);
        GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetColorAlpha(value* initColorAlphaList_[static_cast<int>(Usefulness::HUD_Reticle_Lockon_Out)]);*/

        GetUI(Usefulness::HUD_Reticle_Base)->SetScale(scale, scale);
        GetUI(Usefulness::HUD_Reticle_Base)->SetColorAlpha(value* initColorAlphaList_[static_cast<int>(Usefulness::HUD_Reticle_Base)]);

        GetUI(Usefulness::Vitesse_Reticle)->SetScale(scale, scale);
        GetUI(Usefulness::Vitesse_Reticle)->SetColorAlpha(value* initColorAlphaList_[static_cast<int>(Usefulness::Vitesse_Reticle)]);

        GetUI(Usefulness::HUD_Decoration)->SetScale(scale, scale);
        GetUI(Usefulness::HUD_Decoration)->SetColorAlpha(value* initColorAlphaList_[static_cast<int>(Usefulness::HUD_Decoration)]);

        GetUI(Usefulness::Boss_HP_Gauge_Back)->SetColorAlpha(value * initColorAlphaList_[static_cast<int>(Usefulness::Boss_HP_Gauge_Back)]);
        GetUI(Usefulness::Boss_HP_Gauge_Dmg)->SetColorAlpha(value * initColorAlphaList_[static_cast<int>(Usefulness::Boss_HP_Gauge_Dmg)]);
        GetUI(Usefulness::Boss_HP_Gauge_Main)->SetColorAlpha(value * initColorAlphaList_[static_cast<int>(Usefulness::Boss_HP_Gauge_Main)]);
        GetUI(Usefulness::Boss_Name)->SetColorAlpha(value * initColorAlphaList_[static_cast<int>(Usefulness::Boss_Name)]);

        GetUI(Usefulness::HUD_HP_Gauge_Back)->SetColorAlpha(value* initColorAlphaList_[static_cast<int>(Usefulness::HUD_HP_Gauge_Back)]);
        GetUI(Usefulness::HUD_HP_Gauge_Dmg)->SetColorAlpha(value* initColorAlphaList_[static_cast<int>(Usefulness::HUD_HP_Gauge_Dmg)]);
        GetUI(Usefulness::HUD_HP_Gauge_Main)->SetColorAlpha(value* initColorAlphaList_[static_cast<int>(Usefulness::HUD_HP_Gauge_Main)]);
        GetUI(Usefulness::HUD_Vitesse_Name)->SetColorAlpha(value* initColorAlphaList_[static_cast<int>(Usefulness::HUD_Vitesse_Name)]);

        GetUI(Usefulness::HUD_Boost_Gauge_Back)->SetColorAlpha(value* initColorAlphaList_[static_cast<int>(Usefulness::HUD_Boost_Gauge_Back)]);
        GetUI(Usefulness::HUD_Boost_Gauge_Main)->SetColorAlpha(value* initColorAlphaList_[static_cast<int>(Usefulness::HUD_Boost_Gauge_Main)]);

        //背景だけはゆっくりと消していく
        float alpha = 1.0f - value;
        GetUI(Usefulness::SSU_Back)->SetColorAlpha(alpha);

        if (systemStartUpTimer_ > sSuCompleteTime_)
        {
            systemStartUpTimer_ = 0.0f;

             //システム起動演出後に必要なUIの表示・非表示
            for (auto& s : sSUUpdateSprites_)
            {
                s->SetEnable(false);
            }

            //ヴィテス本体立ち上げ
            if (const auto& p = player_.lock())
            {
                if (const auto& v = p->GetMyVitesse())
                {
                    v->SetIsSystemStart(true);
                }
            }

            //SE停止
            sSUAlwaysSound_->Stop();


            //終了
            isSystemStartUp_ = false;

            systemStartUpStep_ = SSUStep::Init;
        }

    }
        break;
    }

    systemStartUpTimer_ += Time::GetDeltaTime();
}

void GameUIAdmin::ToVitessseUI()
{
    //ヴィテス搭乗中のUI
    GetUI(Usefulness::HUD_Reticle_Base)->SetEnable(true);
    //GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetEnable(true);
    //GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetEnable(true);
    GetUI(Usefulness::Vitesse_Reticle)->SetEnable(true);
    GetUI(Usefulness::HUD_Boost_Gauge_Back)->SetEnable(true);
    GetUI(Usefulness::HUD_Boost_Gauge_Main)->SetEnable(true);
    GetUI(Usefulness::HUD_HP_Gauge_Back)->SetEnable(true);
    GetUI(Usefulness::HUD_HP_Gauge_Dmg)->SetEnable(true);
    GetUI(Usefulness::HUD_HP_Gauge_Main)->SetEnable(true);
    GetUI(Usefulness::HUD_Vitesse_Name)->SetEnable(true);
    GetUI(Usefulness::HUD_Decoration)->SetEnable(true);
    
    //通常のレティクルを消しておく
    GetUI(Usefulness::Reticle)->SetEnable(false);

    isSystemStartUp_ = true;
}

void GameUIAdmin::ToSoldierUI()
{
    //歩兵時のUI
    GetUI(Usefulness::HUD_Reticle_Base)->SetEnable(false);
    //GetUI(Usefulness::HUD_Reticle_Circle)->SetEnable(false);
    GetUI(Usefulness::HUD_Reticle_Lockon_In)->SetEnable(false);
    GetUI(Usefulness::HUD_Reticle_Lockon_Out)->SetEnable(false);
    GetUI(Usefulness::Vitesse_Reticle)->SetEnable(false);
    GetUI(Usefulness::HUD_Boost_Gauge_Back)->SetEnable(false);
    GetUI(Usefulness::HUD_Boost_Gauge_Main)->SetEnable(false);
    GetUI(Usefulness::HUD_HP_Gauge_Back)->SetEnable(false);
    GetUI(Usefulness::HUD_HP_Gauge_Dmg)->SetEnable(false);
    GetUI(Usefulness::HUD_HP_Gauge_Main)->SetEnable(false);
    GetUI(Usefulness::HUD_Vitesse_Name)->SetEnable(false);
    GetUI(Usefulness::HUD_Decoration)->SetEnable(false);

    //通常のレティクルをアクティブに
    GetUI(Usefulness::Reticle)->SetEnable(true);
}

void GameUIAdmin::AudioInitialize()
{
    sSUCompleteSound_ =  actor_->AddComponent<AudioSource>();
    sSUCompleteSound_->SetAssetAudioIndex(AudioIndex::System_Start_Up);
    sSUCompleteSound_->SetActiveDistanceAttenuation(false);

    sSUAlwaysSound_ = actor_->AddComponent<AudioSource>();
    sSUAlwaysSound_->SetAssetAudioIndex(AudioIndex::System_Start_Up_Always);
    sSUAlwaysSound_->SetActiveDistanceAttenuation(false);

    piSound_ =  actor_->AddComponent<AudioSource>();
    piSound_->SetAssetAudioIndex(AudioIndex::Pi);
    piSound_->SetActiveDistanceAttenuation(false);
}

void GameUIAdmin::UpdateBulletHit()
{
    GetUI(Usefulness::Bullet_Hit)->FadeOut(0.0f, bulletHitFadeOutSpeed_);
}
