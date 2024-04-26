#pragma once
#include "DXSystem.h"
#include <memory>


namespace AbyssEngine
{
    class AssetManager;
    class RenderManager;
    class SceneManager;
    class Input;

    class Engine
    {
    public:
        Engine();
        ~Engine();

        static void Update();

        //���C���E�B���h�E�̃n���h�����擾����
        static void GetHandle(UINT msg, WPARAM wParam, LPARAM lParam);


        static std::unique_ptr<AssetManager> assetManager_;
        static std::unique_ptr<RenderManager> renderManager_;
        static std::unique_ptr<SceneManager> sceneManager_;
        static std::unique_ptr<Input> inputManager_;

    private:
        static void DrawDebug();

    };

    class Time
    {
    public:
        static float deltaTime_;
        static float timeScale_;
    };
}

