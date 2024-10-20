#pragma once
#include "ScriptComponent.h"

namespace AbyssEngine
{
    class SpriteRenderer;
}


class SceneTransitionCollider : public AbyssEngine::ScriptComponent
{
public:
    void Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)override;

    void OnCollision(const std::shared_ptr<AbyssEngine::Collider>& collision, AbyssEngine::Collision::IntersectionResult result);

    void SetTransSceneName(const std::string& name) { transSceneName_ = name; }

private:
    //�J�ڐ�̃V�[����
    std::string transSceneName_ = "Test";

    std::shared_ptr<AbyssEngine::SpriteRenderer> spriteRenderer_;
};
