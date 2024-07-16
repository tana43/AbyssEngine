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

        //���C���E�B���h�E�̃n���h�����擾����
        static void GetHandle(UINT msg, WPARAM wParam, LPARAM lParam);


        static std::unique_ptr<AssetManager> assetManager_;
        static std::unique_ptr<RenderManager> renderManager_;
        static std::unique_ptr<SceneManager> sceneManager_;
        static std::unique_ptr<ScriptComponentManager> scriptComManager_;
        static std::unique_ptr<Input> inputManager_;

    private:
        static void DrawDebug();

        //�h���b�O���Ă��鎞�̓}�E�X�J�[�\������ʒ[�ɓ��B����Ɣ��Α��֔�΂���鏈��
        static void MouseDragUnrelenting();
    };

    class Time
    {
    public:
        static float deltaTime_;
        static float timeScale_;
    };

}

