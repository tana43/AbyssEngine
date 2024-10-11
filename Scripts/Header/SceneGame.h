#pragma once
#include "Scene.h"

class SceneGame : public AbyssEngine::Scene
{
private:
    void Initialize()override;
    void Update()override;
    void DrawImGui()override;
};

