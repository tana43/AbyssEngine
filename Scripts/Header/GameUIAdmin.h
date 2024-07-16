#pragma once
#include "Component.h"

namespace AbyssEngine
{
    class Actor;
}

//ゲームシーンに関係するUIを管理するコンポーネント
class GameUIAdmin : public AbyssEngine::Component
{
public:
    GameUIAdmin() {}
    ~GameUIAdmin() {}

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);
    void 
};

