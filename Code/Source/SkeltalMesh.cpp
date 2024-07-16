#include "SkeletalMesh.h"
#include "Actor.h"
#include "Engine.h"
//#include "FbxMeshData.h"
#include "GltfSkeletalMesh.h"
#include "RenderManager.h"
#include "AssetManager.h"
#include "Animator.h"
#include "StaticMesh.h"
#include "ShapeRenderer.h"

#include "imgui/imgui.h"

using namespace AbyssEngine;
using namespace std;

void SkeletalMesh::Initialize(const std::shared_ptr<Actor>& actor)
{
    //マネージャーの登録と初期化
    actor_ = actor;
    transform_ = actor->GetTransform();

	//モデル読み込み
	const auto it = Engine::assetManager_->cacheSkeletalMesh_.find(filePath_);
	if (it != Engine::assetManager_->cacheSkeletalMesh_.end())
	{
		model_ = it->second;
	}
	else
	{
		//一度も読み込まれていないモデルなら新たに読み込み、アセットマネージャーに登録
		model_ = make_shared<GltfSkeletalMesh>(filePath_.c_str());
		Engine::assetManager_->cacheSkeletalMesh_[filePath_] = model_;
	}

	//animatorコンポーネント追加(初期化処理も)
	animator_ = actor->AddComponent<Animator>();
	animator_->LatterInitialize(static_pointer_cast<SkeletalMesh>(shared_from_this()));

	//レンダラーマネージャーに登録
	SetActive(true);
}

void SkeletalMesh::Render()
{
	model_->Draw(DrawPass::Opaque, transform_->GetWorldMatrix(), animator_->GetAnimatedNodes());
	model_->Draw(DrawPass::Transmission, transform_->GetWorldMatrix(), animator_->GetAnimatedNodes());


#if _DEBUG
	//バウンディングボックス表示
	Engine::renderManager_->shapeRenderer_->DrawBox(
		boundingBox_.Center,
		Vector3(0, 0, 0),
		boundingBox_.Extents,
		Vector4(0, 1, 1, 1)
	);
#endif // _DEBUG
}

void SkeletalMesh::RenderShadow()
{
	model_->CastShadow(transform_->GetWorldMatrix(), animator_->GetAnimatedNodes());
}

//void SkeletalMesh::AppendAnimation(const std::string& filename)
//{
//	model_->AppendAnimation(filename);
//	animatedNodes_ = model_->nodes_;
//}
//
//void SkeletalMesh::AppendAnimations(const std::vector<std::string>& filenames)
//{
//	model_->AppendAnimations(filenames);
//	animatedNodes_ = model_->nodes_;
//}

void SkeletalMesh::RecalculateFrame()
{
	//アニメーション更新
	animator_->AnimatorUpdate();

	transform_->CalcWorldMatrix();//行列更新

	//バウンディングボックス更新
	{
		DirectX::XMVECTOR MinValue, MaxValue;
		MinValue = DirectX::XMLoadFloat3(&model_->minValue_);
		MaxValue = DirectX::XMLoadFloat3(&model_->maxValue_);
		ComputeTransformedBounds(MinValue, MaxValue, transform_->GetWorldMatrix());
		DirectX::XMStoreFloat3(&minValue_, MinValue);
		DirectX::XMStoreFloat3(&maxValue_, MaxValue);

		boundingBox_ = ConvertToDXBoundingBox(minValue_, maxValue_);
	}
}


bool SkeletalMesh::FrustumCulling(const DirectX::BoundingFrustum& frustum)
{
	DirectX::ContainmentType ret = boundingBox_.Contains(frustum);
	if (ret >= DirectX::INTERSECTS)
	{
		return true;
	}
	return false;
}

bool SkeletalMesh::ShadowCulling(const DirectX::BoundingBox& box)
{
	DirectX::ContainmentType ret = boundingBox_.Contains(box);
	if (ret >= DirectX::INTERSECTS)
	{
		return true;
	}
	return false;
}

bool SkeletalMesh::DrawImGui()
{
	
	if (ImGui::TreeNode("Skeletal Mesh"))
	{
		ImGui::Checkbox("Enabled",&enabled_);

		ImGui::DragFloat("Emissive Intensity", &model_->primitiveConstants_->data_.emissiveIntensity_, 0.01f, 0.0f);
		ImGui::DragFloat("Image Based Lighting Intensity", &model_->primitiveConstants_->data_.imageBasedLightingIntensity_, 0.01f, 0.0f);

		ImGui::TreePop();
	}

	return true;
}

void SkeletalMesh::PlayAnimation(int animIndex,bool loop)
{
	
}

void SkeletalMesh::SocketAttach(const std::shared_ptr<StaticMesh>& attachModel, const char* socketName)
{
	auto& socketData = attachModel->GetSocketData();

	attachModel->isAttached_ = true;
	socketData.attachedSocketName_ = socketName;
	socketData.attachedMesh_ = static_pointer_cast<SkeletalMesh>(shared_from_this());
}

const Matrix& AbyssEngine::SkeletalMesh::FindSocket(const char* socketName)
{
	auto& animNodes = GetAnimator()->GetAnimatedNodes();
	std::vector<GeometricSubstance::Node>::iterator node;
	for (auto it = animNodes.begin(); it != animNodes.end(); it++)
	{
		if (it->name_ == socketName)
		{
			node = it;
			break;
		}
	}
	if (node != animNodes.end())
	{
		using namespace DirectX;
		Matrix dxUe5 = XMMatrixSet(-1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1); // LHS Y-Up Z-Forward(DX) -> LHS Z-Up Y-Forward(UE5) 
		Matrix ue5Gltf = XMMatrixSet(1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1); // LHS Z-Up Y-Forward(UE5) -> RHS Y-Up Z-Forward(glTF) 
		Matrix boneTransform = XMLoadFloat4x4(&node->globalTransform_);

		//return dxUe5 * ue5Gltf * boneTransform;
		return boneTransform;
	}

	_ASSERT_EXPR(false, L"同じ名前のソケットが見つかりませんでした");
}

void SkeletalMesh::SetActive(const bool value)
{
	if (value)
	{
		if (!isCalled_)
		{
			Engine::renderManager_->Add(static_pointer_cast<SkeletalMesh>(shared_from_this()));
			isCalled_ = true;
		}
	}
}
//
//void GltfSkeltalMesh::Initialize(const std::shared_ptr<Actor>& actor)
//{
//	//マネージャーの登録と初期化
//	actor_ = actor;
//	transform_ = actor->GetTransform();
//
//	model_ = make_unique<GltfModel>(DXSystem::device_.Get(), filePath_.c_str());
//
//	//レンダラーマネージャーに登録
//	SetActive(true);
//}
//
//void GltfSkeltalMesh::Render()
//{
//	static std::vector<GltfModel::Node> animatedNodes = model_->nodes_;
//	static float time{ 0 };
//	model_->Animate(0, time += Time::deltaTime_, animatedNodes);
//
//	model_->Render(DXSystem::deviceContext_.Get(), transform_->CalcWorldMatrix(), animatedNodes/*UNIT.37*/);
//}
//
//void GltfSkeltalMesh::SetActive(const bool value)
//{
//	if (value)
//	{
//		if (!isCalled_)
//		{
//			Engine::renderManager_->Add(static_pointer_cast<GltfSkeltalMesh>(shared_from_this()));
//			isCalled_ = true;
//		}
//	}
//}
