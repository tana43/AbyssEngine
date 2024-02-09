#include "Engine.h"

using namespace AbyssEngine;

Engine::Engine()
{
}

Engine::~Engine()
{
    DXSystem::Release();
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
