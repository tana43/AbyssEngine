#include "SceneTransitionCollider.h"
#include "GameCollider.h"
#include "Actor.h"
#include "SceneManager.h"
#include "Engine.h"
#include "SpriteRenderer.h"
#include "Actor.h"

using namespace AbyssEngine;

void SceneTransitionCollider::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    ScriptComponent::Initialize(actor);

    spriteRenderer_ = actor_->AddComponent<SpriteRenderer>("./Assets/Images/Proto_Load_01.png");
    spriteRenderer_->SetEnable(false);

    //���̃R���C�_�[�Ƀq�b�g����ƃV�[�����J�ڂ���
    AddAttackCollider(Vector3::Zero,2.0f);
}

void SceneTransitionCollider::OnCollision(const std::shared_ptr<Collider>& collision, Collision::IntersectionResult result)
{
    //����������炢����p�̃R���C�_�[�Ƀv���C���[�̃^�O�����Ă���Ȃ�V�[����ύX����
    if (collision->GetTag() & static_cast<unsigned int>(Collider::Tag::Player))
    {
        spriteRenderer_->SetEnable(true);

        Engine::sceneManager_->SetNextScene(transSceneName_);
    }
}
