#pragma once
#include "Scene.h"
#include "Effect.h"

namespace AbyssEngine
{
    class TestScene : public Scene
    {
    private:
        void Initialize()override;
        void Update()override;
        void DrawImGui()override;

        std::unique_ptr<Effect> swordEfe_;
    };
}

