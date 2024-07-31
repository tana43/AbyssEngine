#pragma once
#include "BaseEnemy.h"

namespace AbyssEngine
{
    class SkeletalMesh;
}

class EnemyBot : public BaseEnemy
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

    
private:
    std::shared_ptr<AbyssEngine::SkeletalMesh> model_;

    
};

