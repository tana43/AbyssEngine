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
        Reticle,//���e�B�N��
        HUD_Reticle_Base,//HUD�@��{�I�ɓ����Ȃ�UI
        //HUD_Reticle_Circle,//HUD�@��ɉ�]���������UI
        Vitesse_Reticle,//���B�e�X��p�̃��e�B�N��
        HUD_Reticle_Lockon_Out,//���b�N�I�����ɒʏ탌�e�B�N���Ɛ؂�ւ��ĕ\��������
        HUD_Reticle_Lockon_In,//���b�N�I�����̂ݏ�ɉ�]������
    };

public:
    GameUIAdmin() {}
    ~GameUIAdmin() {}

    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;
    
    void UpdateAfter()override;

    //TPS���̃v���C���[
    void UiUpdatePlayerSoldier();

    //���B�e�X���掞��
    void UiUpdateVitesse();

    //�v���C���[��ݒ�
    void SetPlayer(const std::shared_ptr<Soldier>& p) { player_ = p; }

    const std::shared_ptr<AbyssEngine::SpriteRenderer>& GetUI(Usefulness index) const { return spriteList_[static_cast<int>(index)]; }

private:
    void LockonUiMove();

private:
    std::vector<std::shared_ptr<AbyssEngine::SpriteRenderer>> spriteList_;

    //�e��f�[�^���Q�Ƃ��邽�߂̃v���C���[
    std::shared_ptr<Soldier> player_;

    static AbyssEngine::Vector2 screenCenter_;

    //�G�����b�N�����Ƃ��Ƀ��e�B�N�����ړ�����O�̒l
    AbyssEngine::Vector2 reticleBeforeMovePos_ = {};

    //�^�[�Q�b�g�����b�N�����ۂ�UI���ړ������邽�߂Ɏg�p����^�C�}�[
    float lockonMovetimer_ = 0.0f;
    //���b�N�ύX�̍ۂ�UI�̈ړ��Ɋ|���鎞��
    float Lockon_Move_Time = 0.1f;
};

