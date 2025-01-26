#pragma once
#include "ScriptComponent.h"

#include <vector>

namespace AbyssEngine
{
    class Actor;
    class SpriteRenderer;
    class AudioSource;
}

class Soldier;
class BossMech;

//ゲームシーンに関係するUIを管理するコンポーネント
class GameUIAdmin : public AbyssEngine::ScriptComponent
{
public:
    //使用用途で要素番号を管理するためのEnum
    enum class Usefulness
    {
        Boarding,//搭乗

        Reticle,//レティクル
        Bullet_Hit,//弾がヒットした際に

        HUD_Reticle_Base,//HUD　基本的に動かないUI
        //HUD_Reticle_Circle,//HUD　常に回転させる厚めUI
        Vitesse_Reticle,//ヴィテス専用のレティクル
        HUD_Reticle_Lockon_Out,//ロックオン時に通常レティクルと切り替えて表示させる
        HUD_Reticle_Lockon_In,//ロックオン時のみ常に回転させる

        HUD_Decoration,//HUD 現在は装飾として配置しているがターゲットの範囲外としておいてもいいなあと思ってる

        HUD_Boost_Gauge_Back,//ブーストゲージ背景
        HUD_Boost_Gauge_Main,//ブーストゲージ本体

        Boss_HP_Gauge_Back,//ボスのHPゲージ背景
        Boss_HP_Gauge_Dmg,//ボスのHPダメージ部分
        Boss_HP_Gauge_Main,//ボスのHPゲージ本体
        Boss_Name,//ボスの名前

        HUD_HP_Gauge_Back,//プレイヤーのHPゲージ背景
        HUD_HP_Gauge_Dmg,//プレイヤーのHPダメージ部分
        HUD_HP_Gauge_Main,//プレイヤーのHPゲージ本体

        HUD_Vitesse_Name,//Vitesseの正式名称表示


        //------------起動演出-------------
        SSU_Back,

        SSU_Energy_Output,
        SSU_Drive_System,
        SSU_Weapon_System,
        SSU_Boost_System,
        SSU_Alert_System,

        SSU_Energy_Output_Gauge,
        SSU_Drive_System_Gauge,
        SSU_Weapon_System_Gauge,
        SSU_Boost_System_Gauge,
        SSU_Alert_System_Gauge,

        SSU_Frame,

        System_Start_Up,

        //--------------Error---------------
        HUD_Error_Acoustic_Censor,
        HUD_Error_Missing_Parts,
        HUD_Error_Movement,
        HUD_Error_Overheat,
        HUD_Error_Stagger,
        HUD_Error_Weapon,
    };

public:
    GameUIAdmin() {}
    ~GameUIAdmin() {}

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    
    void DrawImGui()override;

    void UpdateAfter()override;

    //TPS時のプレイヤー
    void UiUpdatePlayerSoldier();

    //ヴィテス搭乗時の
    void UiUpdateVitesse();

    //ブーストゲージ
    void UiUpdateBoostGauge();

    //ヴィテスのHP表示
    void UiUpdateVitesseHpGauge();

    //ボスのHPゲージ更新
    void UiUpdateBossHpGauge();

    //プレイヤーを設定
    void SetPlayer(const std::shared_ptr<Soldier>& p) { player_ = p; }

    //ボスを設定
    void SetBoss(const std::shared_ptr<BossMech>& p) { boss_ = p; }

    const std::shared_ptr<AbyssEngine::SpriteRenderer>& GetUI(Usefulness index) const { return spriteList_[static_cast<int>(index)]; }

    //弾がヒットしたときに外部で呼ぶ関数
    void BulletHit();

private:
    void LockonUiMove();

    // 読み込み
    void LoadSprite();

    //起動演出で出すUIを表示
    void LaunchSSUWindow(Usefulness ui);

    //起動演出で出すUIを非表示
    void CloseSSUWindow(Usefulness ui);

    //すべての起動演出で出すUIを非表示
    void AllStopSSUWindow();

    //システム起動演出に使うUI表示の更新
    void UpdateSSUWindow();

    //システム起動演出
    void UpdateSSU();

    //VitesseのUIを切り替える
    void ToVitessseUI();

    //歩兵時のUIに切り替える
    void ToSoldierUI();

    void AudioInitialize();

    //弾丸が当たった際にフェードアウトするための関数
    void UpdateBulletHit();

private:
    std::vector<std::shared_ptr<AbyssEngine::SpriteRenderer>> spriteList_;

    //各種データを参照するためのプレイヤー
    std::weak_ptr<Soldier> player_;

    //ボス
    std::weak_ptr<BossMech> boss_;

    //ヴィテスに搭乗しているか
    bool isBoardVitesse_ = false;

    static AbyssEngine::Vector2 screenCenter_;

    //敵をロックしたときにレティクルを移動する前の値
    AbyssEngine::Vector2 reticleBeforeMovePos_ = {};

    //ターゲットをロックした際にUIを移動させるために使用するタイマー
    float lockonMovetimer_ = 0.0f;
    //ロック変更の際にUIの移動に掛ける時間
    float Lockon_Move_Time = 0.1f;

    //ブーストゲージの点滅用タイマー
    float boostGaugeBlinkTimer_ = 0.0f;

    //システム起動時に使用するタイマー
    enum class SSUStep
    {
        Init,
        WindowLaunch,
        Loading,
        WindowClose,
        Complete,
    };
    bool isSystemStartUp_ = false;
    float sSuLaunchTime_ = 0.1f;
    float sSuLoadingTime_ = 0.3f;
    float sSuCloseTime_ = 0.2f;
    float sSuCompleteTime_ = 0.3f;
    float systemStartUpTimer_ = 0.0f;
    std::vector<Usefulness>  systemStartUpUIs_;
    SSUStep systemStartUpStep_ = SSUStep::Init;

    //起動演出で使用するUIの数
    static constexpr int Max_SSU_Ui = 18;
    bool errorActive_[Max_SSU_Ui] = {};
    float errorTimer_[Max_SSU_Ui];
    float SSUAnimationSpeed_ = 9.5f;
    //float errorColorAlpha_ = 0.8f;

    //システム起動中にのみ表示するUI
    std::vector<AbyssEngine::SpriteRenderer*> sSUUpdateSprites_;

    //システム起動完了後に表示するUI
    std::vector<AbyssEngine::SpriteRenderer*> sSUCompleteLaunchSprites_;

    //初期の透明度だけ登録しておく
    std::vector<float> initColorAlphaList_;

    //システム起動完了のSE
    std::shared_ptr<AbyssEngine::AudioSource> sSUCompleteSound_;

    //システム起動中のSE
    std::shared_ptr<AbyssEngine::AudioSource> sSUAlwaysSound_;

    //ピッって音
    std::shared_ptr<AbyssEngine::AudioSource> piSound_;

    //ダメージを受けてから経過した時間
    float bossDamagedTimer_;
    float vitesseDamagedTimer_;

    //前回のHP
    float preBossHp_;
    float preVitesseHp_;

    //ダメージを受けてから何秒間赤いUIを表示するか
    float damageUiHoldTime = 2.0f;

    //弾丸を当てたときに出現するＵＩが消えていく速度
    float bulletHitFadeOutSpeed_ = 3.0f;
};

