#pragma once
#include "ScriptComponent.h"
#include <vector>

namespace AbyssEngine
{
    //Update����������
    class ScriptComponentManager
    {
    public:
        ScriptComponentManager() = default;

    public:
        void Update();

        void AddScriptCom(const std::shared_ptr<ScriptComponent>& com);

        void Clear();
    private:
        std::vector<std::shared_ptr<ScriptComponent>> scriptComList_;

        friend class Engine;
    };
}

