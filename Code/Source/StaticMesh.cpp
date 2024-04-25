#include "StaticMesh.h"
#include "Actor.h"
#include "StaticMeshBatching.h"
#include "Engine.h"
#include "RenderManager.h"

using namespace AbyssEngine;
using namespace std;

void StaticMesh::Initialize(const std::shared_ptr<Actor>& actor)
{
    //�}�l�[�W���[�̓o�^�Ə�����
    actor_ = actor;
    transform_ = actor->GetTransform();

    model_ = make_unique<StaticMeshBatching>(filePath_.c_str());

    //�����_���[�}�l�[�W���[�ɓo�^
    SetActive(true);

}

void StaticMesh::Render()
{
    model_->Render(transform_->CalcWorldMatrix());
}

void StaticMesh::SetActive(const bool value)
{
    if (value)
    {
        if (!isCalled_)
        {
            Engine::renderManager_->Add(static_pointer_cast<StaticMesh>(shared_from_this()));
            isCalled_ = true;
        }
    }
}
