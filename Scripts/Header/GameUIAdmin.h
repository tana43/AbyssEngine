#pragma once
#include "ScriptComponent.h"

#include <vector>

namespace AbyssEngine
{
    class Actor;
    class SpriteRenderer;
}

class Soldier;

//�Q�[���V�[���Ɋ֌W����UI���Ǘ�����R���|�[�l���g
class GameUIAdmin : public AbyssEngine::ScriptComponent
{
public:
    //�g�p�p�r�ŗv�f�ԍ����Ǘ����邽�߂�Enum
    enum class Usefulness
    {
        Boarding,//����
    };

public:
    GameUIAdmin() {}
    ~GameUIAdmin() {}

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    
    void UpdateAfter()override;

    //�v���C���[��ݒ�
    void SetPlayer(const std::shared_ptr<Soldier>& p) { player_ = p; }

private:
    std::vector<std::shared_ptr<AbyssEngine::SpriteRenderer>> spriteList_;

    //�e��f�[�^���Q�Ƃ��邽�߂̃v���C���[
    std::shared_ptr<Soldier> player_;
};

