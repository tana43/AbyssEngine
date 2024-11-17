#pragma once
#include "ScriptComponent.h"

namespace AbyssEngine
{
    class SpriteRenderer;
}

class BossMech;

class GameHost : public AbyssEngine::ScriptComponent
{
public:
    GameHost() {}
    ~GameHost() {}

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    void Update()override;

    void SetBoss(const std::shared_ptr<BossMech>& boss) { boss_ = boss; }


    static bool gameClear_;
private:
    std::shared_ptr<BossMech> boss_;

    std::shared_ptr<AbyssEngine::SpriteRenderer> spriteRenderer_;
    std::shared_ptr<AbyssEngine::SpriteRenderer> loadSpriteRenderer_;

    float toTitleTimer_;
};

