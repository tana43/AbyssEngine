#pragma once
#include "DXSystem.h"
#include <memory>

namespace AbyssEngine
{
    class AssetManager;

    class Engine
    {
    public:
        Engine();
        ~Engine();

        static void Update();

        //���C���E�B���h�E�̃n���h�����擾����
        static void GetHandle(UINT msg, WPARAM wParam, LPARAM lParam);


        static std::unique_ptr<AssetManager> assetManager_;
    };

    class Time
    {
    public:
        static float deltaTime_;
        static float timeScale_;
    };
}

