#pragma once
#include "DXSystem.h"
#include <memory>


namespace AbyssEngine
{
    class AssetManager;
    class RenderManager;
    class SceneManager;
    class ScriptComponentManager;
    class Input;

    class Engine
    {
    public:
        Engine();
        ~Engine();

        static void Update();

        //メインウィンドウのハンドルを取得する
        static void GetHandle(UINT msg, WPARAM wParam, LPARAM lParam);


        static std::unique_ptr<AssetManager> assetManager_;
        static std::unique_ptr<RenderManager> renderManager_;
        static std::unique_ptr<SceneManager> sceneManager_;
        static std::unique_ptr<ScriptComponentManager> scriptComManager_;
        static std::unique_ptr<Input> inputManager_;

    private:
        static void DrawDebug();

        //ドラッグしている時はマウスカーソルが画面端に到達すると反対側へ飛ばされる処理
        static void MouseDragUnrelenting();
    };

    class Time
    {
    public:
        static float deltaTime_;
        static float timeScale_;
    };

}

