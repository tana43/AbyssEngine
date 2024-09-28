#pragma once
#include "ScriptComponent.h"

#include <vector>

namespace AbyssEngine
{
    class Actor;
    class SpriteRenderer;
}

class Soldier;

//ゲームシーンに関係するUIを管理するコンポーネント
class GameUIAdmin : public AbyssEngine::ScriptComponent
{
public:
    //使用用途で要素番号を管理するためのEnum
    enum class Usefulness
    {
        Boarding,//搭乗
        Reticle,//レティクル
        HUD_Reticle_Base,//HUD　基本的に動かないUI
        //HUD_Reticle_Circle,//HUD　常に回転させる厚めUI
        Vitesse_Reticle,//ヴィテス専用のレティクル
        HUD_Reticle_Lockon_Out,//ロックオン時に通常レティクルと切り替えて表示させる
        HUD_Reticle_Lockon_In,//ロックオン時のみ常に回転させる
    };

public:
    GameUIAdmin() {}
    ~GameUIAdmin() {}

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    
    void UpdateAfter()override;

    //TPS時のプレイヤー
    void UiUpdatePlayerSoldier();

    //ヴィテス搭乗時の
    void UiUpdateVitesse();

    //プレイヤーを設定
    void SetPlayer(const std::shared_ptr<Soldier>& p) { player_ = p; }

private:
    std::vector<std::shared_ptr<AbyssEngine::SpriteRenderer>> spriteList_;

    //各種データを参照するためのプレイヤー
    std::shared_ptr<Soldier> player_;
};

