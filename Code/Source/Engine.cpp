#include "Engine.h"
#include "AssetManager.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "Scene.h"

using namespace AbyssEngine;
using namespace std;

unique_ptr<AssetManager>    Engine::assetManager_;
unique_ptr<RenderManager>   Engine::renderManager_;
unique_ptr<SceneManager>    Engine::sceneManager_;

Engine::Engine()
{
    //各マネージャーの生成
    sceneManager_ = make_unique<SceneManager>();
    assetManager_ = make_unique<AssetManager>();
    renderManager_ = make_unique<RenderManager>();
}

Engine::~Engine()
{
    DXSystem::Release();
    assetManager_.reset();
    renderManager_.reset();
    sceneManager_.reset();
}

void Engine::Update()
{
    DXSystem::Clear();

    sceneManager_->Update();

    renderManager_->Render();

    DXSystem::Flip();
}

void Engine::GetHandle(UINT msg, WPARAM wParam, LPARAM lParam)
{
   /* switch (msg)
    {
    case WM_SETFOCUS:
        Cursor::window_focus = true;
        break;
    case WM_KILLFOCUS:
        Cursor::window_focus = false;
        break;
    default:
        break;
    }*/
}

float Time::deltaTime_;
float Time::timeScale_;
