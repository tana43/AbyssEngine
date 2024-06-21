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
    a->AddComponent<StaticMesh>(modelPath.c_str());
    std::string collisionModelPath = modelPath + "_Collision";
    const auto& mesh = a->AddComponent<MeshCollider>(collisionModelPath.c_str());
    a->SetParent(actor_);

    meshColliders_.emplace_back(mesh);
}

void Stage::RegisterTriangles()
{
    for (const auto& model : meshColliders_)
    {
        for (const auto& mesh : model->meshes_)
        {
            for (const auto& subset : mesh.subsets)
            {
                for (size_t i = 0; i < subset.positions_.size(); i++)
                {

                }
            }
        }
    }
}
