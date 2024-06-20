#include "Stage.h"
#include "Actor.h"
#include "Engine.h"
#include "SceneManager.h"

using namespace AbyssEngine;

void Stage::Initialize(const std::shared_ptr<AbyssEngine::Actor>& actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();
}

void Stage::AddStageModel(const std::string& actorName, const std::string& modelPath)
{
    const auto& a = Engine::sceneManager_->GetActiveScene().InstanceActor(actorName);
    const auto& mesh = a->AddComponent<StaticMesh>(modelPath.c_str());
    a->SetParent(actor_);

    models_.emplace_back(mesh);
}

void Stage::RegisterTriangles()
{
    for (const auto& model : models_)
    {
        for (const auto& mesh : model->meshes_)
        {
            for (const auto& primitive : mesh.primitives_)
            {
                primitive.vertexBufferViews_.at("POSITION");

            }
        }
    }
}
