#pragma once
#include "Scene.h"

namespace AbyssEngine
{
    class SpriteRenderer;
}

class SceneTitle : public AbyssEngine::Scene 
{
private:
    void Initialize()override;
    void Update()override;
    void DrawImGui()override;
    void Finalize()override;

private:
    std::weak_ptr<AbyssEngine::SpriteRenderer> loadSprite_;
};

