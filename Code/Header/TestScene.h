#pragma once
#include "Scene.h"

namespace AbyssEngine
{
    class TestScene : public Scene
    {
    private:
        void Initialize()override;
        void Update()override;
    };
}

