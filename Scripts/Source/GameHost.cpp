#include "GameHost.h"
#include "Actor.h"
#include "SpriteRenderer.h"
#include "DxSystem.h"
#include "BossMech.h"
#include "Input.h"
#include "SceneManager.h"
#include "Engine.h"

using namespace AbyssEngine;

bool GameHost::gameClear_ = false;

void GameHost::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    ScriptComponent::Initialize(actor);


    loadSpriteRenderer_ = actor->AddComponent<SpriteRenderer>("./Assets/Images/NowLoading.png");
    loadSpriteRenderer_->SetEnable(false);


    spriteRenderer_ = actor->AddComponent<SpriteRenderer>("./Assets/Images/UI/GameText/Text_MissionComplete.png");
    D3D11_VIEWPORT viewport;
    DXSystem::GetViewport(1, &viewport);
    Vector2 screenCenter_ = { viewport.Width / 2, viewport.Height / 2 };
    spriteRenderer_->SetOffsetPosition({ screenCenter_ });
    spriteRenderer_->SetPivot({ 0.5f,0.0f });
    //spriteRenderer_->SetScale({ 0.5f,0.5f });

    spriteRenderer_->SetEnable(false);

    gameClear_ = false;


}

void GameHost::Update()
{
    //ボスを撃破したならゲームクリア
    if (!gameClear_)
    {
        if (boss_->GetIsDead())
        {
            gameClear_ = true;
            spriteRenderer_->SetEnable(true);
        }
    }

    //ゲームクリアしていてエンターが押されたらタイトルへ
    if (gameClear_)
    {
        if (Input::GameSupport::GetStartButton())
        {
            Engine::sceneManager_->SetNextScene("Title");
            loadSpriteRenderer_->SetEnable(true);
        }
    }

}
