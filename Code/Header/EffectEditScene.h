#pragma once
#include "Scene.h"

namespace AbyssEngine
{
    class ComputeParticleEmitter;
}

class EffectEditScene : public AbyssEngine::Scene
{
private:
    void Initialize()override;
    void DrawImGuiAlways()override;
};

