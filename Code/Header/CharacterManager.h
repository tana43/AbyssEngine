#pragma once
#include "Character.h"
#include <vector>

namespace AbyssEngine
{
    class CharacterManager
    {
    public:
        CharacterManager() = default;

    public:
        void Update();

        void AddCharacter(const std::shared_ptr<Character>& chara);

        void Clear();
    private:
        std::vector<std::shared_ptr<Character>> characterList_;

        friend class Engine;
    };
}

