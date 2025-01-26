#pragma once
#include "Scene.h"
#include "Effect.h"

namespace AbyssEngine
{
    class SpriteRenderer;

    class TestScene : public Scene
    {
    private:
        void Initialize()override;
        void Update()override;
        void DrawImGui()override;
        void Finalize()override;

        std::unique_ptr<Effect> swordEfe_;

        std::weak_ptr<SpriteRenderer> fadeOutSprite_;
    };
}

