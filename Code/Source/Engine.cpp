#include "Engine.h"
#include "AssetManager.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "Input.h"
#include "Scene.h"
#include "Misc.h"
#include "Bloom.h"
#include "Skybox.h"
#include "CascadedShadowMap.h"
#include "../External/imgui/ImGuiCtrl.h"

using namespace AbyssEngine;
using namespace std;

unique_ptr<AssetManager>    Engine::assetManager_;
unique_ptr<RenderManager>   Engine::renderManager_;
unique_ptr<SceneManager>    Engine::sceneManager_;
unique_ptr<Input>           Engine::inputManager_;

Engine::Engine()
{
    //各マネージャーの生成
    sceneManager_ = make_unique<SceneManager>();
    assetManager_ = make_unique<AssetManager>();
    renderManager_ = make_unique<RenderManager>();
    inputManager_ = make_unique<Input>();

    //ImGui初期化
    IMGUI_CTRL_INITIALIZE(DXSystem::hwnd_, DXSystem::device_.Get(), DXSystem::deviceContext_.Get());
}

Engine::~Engine()
{
    renderManager_.reset();
    sceneManager_->Exit();
    sceneManager_.reset();
    assetManager_->Exit();
    assetManager_.reset();

    //ImGui後処理
    IMGUI_CTRL_UNINITIALIZE();

    DXSystem::Release();
}

void Engine::Update()
{
    //ImGui更新
    IMGUI_CTRL_CLEAR_FRAME();

    DXSystem::Clear();

    inputManager_->Update();

    sceneManager_->Update();

    renderManager_->Render();

    //ImGui描画
    DrawDebug();
    IMGUI_CTRL_DISPLAY();

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

void Engine::DrawDebug()
{
#if _DEBUG
    if (ImGui::BeginMainMenuBar())
    {
        //描画関係の値が対象
        if (ImGui::BeginMenu("Graphics"))
        {
            renderManager_->DrawImGui();

            ImGui::EndMenu();
        }

        //ワールドに存在するアクターを対象
        if (ImGui::BeginMenu("World OutLiner"))
        {
            sceneManager_->DrawImGui();

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
#endif // _DEBUG
}

float Time::deltaTime_;
float Time::timeScale_;
