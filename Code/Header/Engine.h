#pragma once
#include "DXSystem.h"
#include <memory>

namespace AbyssEngine
{
    class Engine
    {
    public:
        Engine();
        ~Engine();

        static void Update();

        //メインウィンドウのハンドルを取得する
        static void GetHandle(UINT msg, WPARAM wParam, LPARAM lParam);
    };

    class Time
    {
    public:
        static float deltaTime_;
        static float timeScale_;
    };
}

