#pragma once
#include "Component.h"

namespace AbyssEngine
{
    class Actor;
}

//�Q�[���V�[���Ɋ֌W����UI���Ǘ�����R���|�[�l���g
class GameUIAdmin : public AbyssEngine::Component
{
public:
    GameUIAdmin() {}
    ~GameUIAdmin() {}

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);
    void 
};

