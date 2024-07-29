#pragma once
#include "BaseEnemy.h"

namespace AbyssEngine
{
    class SkeletalMesh;
}

class EnemyBot : public BaseEnemy
{
public:
    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);
    
private:
    std::shared_ptr<AbyssEngine::SkeletalMesh> model_;
};

