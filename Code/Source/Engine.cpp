#include "Engine.h"
#include "AssetManager.h"

using namespace AbyssEngine;
using namespace std;

unique_ptr<AssetManager> Engine::assetManager_;

Engine::Engine()
{
    //各マネージャーの生成
    assetManager_ = make_unique<AssetManager>();
}

Engine::~Engine()
{
    DXSystem::Release();
    assetManager_.reset();
}

void Engine::Update()
{
    DXSystem::Clear();

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
