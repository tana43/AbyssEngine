#pragma once
#include "BaseEnemy.h"

namespace AbyssEngine
{
    class SkeletalMesh;
}

class BotEnemy : public BaseEnemy
{
public:
    enum class AnimState
    {
        Idle,
        Rolling,
        Walk,
        Jump, 
        Search,
        Attack,
        Attack_Assult,//‘Oi‚µ‚Â‚ÂUŒ‚
    };

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);
    void Update()override;

    void DrawDebug()override;

    //ƒvƒŒƒCƒ„[‚ğõ“G
    bool SearchTarget();

public:

private:
    std::shared_ptr<AbyssEngine::SkeletalMesh> model_;


    //õ“G”ÍˆÍ
    float searchAreaRadius_ = 5.0f;

    //‹^—“I‚È‹–ìŠp@“G‚ª‹ŠE“à‚É‚¢‚é‚©‚ğ”»’f‚·‚é
    float degreeFov_ = 150.0f;

    //ã‚Ì‹–ìŠp‚ğ‚O`‚P‚É³‹K‰»‚µ‚½‚à‚Ì
    float fov_;
};

