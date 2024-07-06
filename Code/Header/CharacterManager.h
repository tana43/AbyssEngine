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

        //�x���X�V�@�`�掞�̍s��v�Z�Ȃǂ��I����Ă���s����
        //void DelayedUpdate();

        void AddCharacter(const std::shared_ptr<Character>& chara);

        void Clear();
    private:
        std::vector<std::shared_ptr<Character>> characterList_;

        friend class Engine;
    };
}

