#include "SceneTransitionCollider.h"
#include "GameCollider.h"
#include "Actor.h"
#include "SceneManager.h"
#include "Engine.h"

using namespace AbyssEngine;

void SceneTransitionCollider::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    ScriptComponent::Initialize(actor);

    //このコライダーにヒットするとシーンが遷移する
    AddAttackCollider(Vector3::Zero,2.0f);
}

void SceneTransitionCollider::OnCollision(const std::shared_ptr<Collider>& collision, Collision::IntersectionResult result)
{
    //当たった喰らい判定用のコライダーにプレイヤーのタグがついているならシーンを変更する
    if (collision->GetTag() & static_cast<unsigned int>(Collider::Tag::Player))
    {
        Engine::sceneManager_->SetNextScene(transSceneName_);
    }
}
