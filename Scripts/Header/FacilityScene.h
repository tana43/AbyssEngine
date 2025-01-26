#pragma once
#include "Scene.h"

namespace AbyssEngine
{
    class SpriteRenderer;
}

class FacilityScene : public AbyssEngine::Scene
{
private:
    void Initialize()override;
    void Update()override;
    void DrawImGui()override;

private:
    std::weak_ptr<AbyssEngine::SpriteRenderer> fadeOutSprite_;
};

