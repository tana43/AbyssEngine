#pragma once
#include "ScriptComponent.h"
#include <vector>

namespace AbyssEngine
{
    //Updateˆ—‚¢‚¯‚é
    class ScriptComponentManager
    {
    public:
        ScriptComponentManager() = default;

    public:
        void Update();

        void AddScriptCom(const std::shared_ptr<ScriptComponent>& com);

        void Clear();
    private:
        std::vector<std::weak_ptr<ScriptComponent>> scriptComList_;

        friend class Engine;
    };
}

