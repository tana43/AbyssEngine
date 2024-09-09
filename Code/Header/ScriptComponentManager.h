#pragma once
#include "ScriptComponent.h"
#include <vector>

namespace AbyssEngine
{
    //UpdateèàóùÇ¢ÇØÇÈ
    class ScriptComponentManager
    {
    public:
        ScriptComponentManager() = default;

    public:
        void Update();

        void AddScriptCom(const std::shared_ptr<ScriptComponent>& com);

        void Clear();

        const std::vector<std::weak_ptr<ScriptComponent>>& GetScriptComList() const { return scriptComList_; }

    private:
        std::vector<std::weak_ptr<ScriptComponent>> scriptComList_;

        friend class Engine;
    };
}

