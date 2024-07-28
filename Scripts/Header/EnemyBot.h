#pragma once
#include "Character.h"

class EnemyBot : public AbyssEngine::Character
{
public:
    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor);
    
};

