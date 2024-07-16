#include "Engine.h"
#include "AssetManager.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "ScriptComponentManager.h"

#include "Input.h"
#include "Scene.h"
#include "Misc.h"
#include "Bloom.h"
#include "Skybox.h"
#include "CascadedShadowMap.h"

#include "DebugRenderer.h"
#include "LineRenderer.h"

#include "StageManager.h"

#include "../External/imgui/ImGuiCtrl.h"

using namespace AbyssEngine;
using namespace std;

unique_ptr<AssetManager>            Engine::assetManager_;
unique_ptr<RenderManager>           Engine::renderManager_;
unique_ptr<SceneManager>            Engine::sceneManager_;
unique_ptr<ScriptComponentManager>  Engine::scriptComManager_;
unique_ptr<Input>                   Engine::inputManager_;

Engine::Engine()
{
    //各マネージャーの生成
    sceneManager_       = make_unique<SceneManager>();
    assetManager_       = make_unique<AssetManager>();
    renderManager_      = make_unique<RenderManager>();
    scriptComManager_   = make_unique<ScriptComponentManager>();
    inputManager_       = make_unique<Input>();

    //ImGui初期化
    IMGUI_CTRL_INITIALIZE(DXSystem::hwnd_, DXSystem::GetDevice(), DXSystem::GetDeviceContext());
}

Engine::~Engine()
{
    renderManager_.reset();
    sceneManager_->Exit();
    sceneManager_.reset();
    assetManager_->Exit();
    assetManager_.reset();
    scriptComManager_.reset();

    //ImGui後処理
    IMGUI_CTRL_UNINITIALIZE();

    DXSystem::Release();
}

void Engine::Update()
{
    ////ImGui更新
    IMGUI_CTRL_CLEAR_FRAME();

    renderManager_->Beginning();

    DXSystem::Clear();

    inputManager_->Update();

    sceneManager_->Update();


    scriptComManager_->Update();

    renderManager_->Render();

    //////ImGui描画
    DrawDebug();
    IMGUI_CTRL_DISPLAY();

    DXSystem::Flip();

#if _DEBUG
    MouseDragUnrelenting();
#endif // _DEBUG

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
    sceneManager_->DrawDebug();

    if (ImGui::BeginMainMenuBar())
    {
        //描画関係の値が対象
        if (ImGui::BeginMenu("Graphics"))
        {
            renderManager_->DrawImGui();

            ImGui::EndMenu();
        }

        //ワールドに存在するアクターが対象
        if (ImGui::BeginMenu("World OutLiner"))
        {
            sceneManager_->DrawWorldOutLinerImGui();

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(sceneManager_->GetActiveScene().name_.c_str()))
        {
            sceneManager_->DrawImGui();

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
#endif // _DEBUG
}

void Engine::MouseDragUnrelenting()
{
#if _DEBUG
    if (Mouse::GetButtonState().leftButton)
    {
        const Vector2 curPos = Vector2(Mouse::GetPosX(), Mouse::GetPosY());

        Vector2 result;
        const Vector2 screen = {
            static_cast<float>(DXSystem::GetScreenWidth()),
            static_cast<float>(DXSystem::GetScreenHeight())
        };

        //カーソルが画面端に近いか
        if (curPos.x <= 1)
        {
            result = { screen.x - 2,curPos.y };
        }
        else if (curPos.x >= screen.x - 1)
        {
            result = { 2,curPos.y };
        }
        else if (curPos.y <= 1)
        {
            result = { curPos.x, screen.y - 2 };
        }
        else if (curPos.y >= screen.y - 1)
        {
            result = { curPos.x, 2 };
        }
        else
        {
            return;
        }

        //変更後の座標をクライアント座標からスクリーン座標に変換
        POINT p = { result.x,result.y };
        ClientToScreen(DXSystem::hwnd_, &p);
        ImGui::TeleportMousePos(ImVec2(p.x, p.y));
        //SetCursorPos(p.x, p.y);
    }
#endif // _DEBUG
}

float Time::deltaTime_;
float Time::timeScale_;
