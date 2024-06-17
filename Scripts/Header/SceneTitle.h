#pragma once
#include "Scene.h"


class SceneTitle : public AbyssEngine::Scene 
{
private:
    void Initialize()override;
    void Update()override;
    void DrawImGui()override;
    void Finalize()override;

};

