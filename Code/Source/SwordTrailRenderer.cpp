#include "SwordTrailRenderer.h"
#include "Engine.h"
#include "Shader.h"
#include "RenderManager.h"
#include "Misc.h"
#include "Actor.h"
#include "DxSystem.h"
#include "Texture.h"
#include "StaticMesh.h"

#include "imgui/imgui.h"

using namespace AbyssEngine;

void AbyssEngine::SwordTrailRenderer::Initialize(const std::shared_ptr<Actor>& actor)
{
	actor_ = actor;
	transform_ = actor->GetTransform();

	// マネージャーに登録
	Engine::renderManager_->Add(std::static_pointer_cast<SwordTrailRenderer>(shared_from_this()));

	// 初期化
	{
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		// 頂点シェーダー
		vertexShader_ = Shader<ID3D11VertexShader>::Emplace(
			"./Resources/Shader/SwordTrailRendererVS.cso",
			inputLayout_.ReleaseAndGetAddressOf(),
			inputElementDesc,
			_countof(inputElementDesc));

		// ピクセルシェーダー
		pixelShader_ = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/SwordTrailRendererPS.cso");

		// 定数バッファ
		constantBuffer_ = std::make_unique<ConstantBuffer<Constant>>();

		// 頂点バッファ
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = sizeof(Vertex) * VertexCapacity;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		HRESULT hr = DXSystem::GetDevice()->CreateBuffer(&desc, nullptr, vertexBuffer_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		//　テクスチャ読み込み
		texture_ = Texture::Load(filePath_);
	}
}

void AbyssEngine::SwordTrailRenderer::Update()
{
	//カラー更新
	constantBuffer_->data_.color_ = color_;

	//　保存していた頂点バッファを１フレーム分ずらす
	{
		for (int x = MAX_POLYGON - 1; x > -1; --x)
		{
			const int previousIndex = x - 1;
			if (previousIndex == -1)
			{
				trailDatas[0] = { {0,0,0},{0,0,0} };

				//軌跡を出さない場合はカラーを０に
				if (!activeTrail_) { trailDatas[0].color_ = Vector4::Zero; }
			}
			else
			{
				trailDatas[x] = trailDatas[previousIndex];
			}
		}
	}

	// 剣の根本と先端の座標を取得し、頂点バッファに保存
	// trailPositions[2][MAX_POLYGON] ← 頂点バッファ
	{
		//オフセット行列作成
		Matrix S = Matrix::CreateScale(Vector3::One);
		Matrix R = Matrix::CreateFromQuaternion(Quaternion::Euler(offsetRotation_));
		Matrix T = Matrix::CreateTranslation(offsetPosition_);
		Matrix offsetMatrix = S * R * T;

		//行列作成
		Matrix worldMatrix = offsetMatrix;
		if (const auto& t = attachedMesh_.lock())
		{
			worldMatrix = worldMatrix * t->GetWorldMatrix();
		}
		//Matrix WorldMatrix = offsetMatrix * transform_->CalcWorldMatrix();

		//前方向
		Vector3 forward = worldMatrix.Forward();
		forward.Normalize();

		//根元の座標算出
		Vector3 tail = {
			worldMatrix.m[3][0],
			worldMatrix.m[3][1],
			worldMatrix.m[3][2]
		};

		//前方向と剣の長さと根元の座標から先端の座標を算出
		Vector3 head = tail + forward * swordRange_;

		//代入
		trailDatas[0].tail_ = tail;
		trailDatas[0].head_ = head;
	}

	{
		static const int tMaxNum = 7;
		const float t[tMaxNum] = { 0.125f, 0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.875f };

		//uv値の算出
		float v = 0;

		AddVertex(trailDatas[0].tail_, { 0.0f,v }, trailDatas[0].color_);
		AddVertex(trailDatas[0].head_, { 1.0f,v }, trailDatas[0].color_);

		//ループ中に増えるUVの値 （Vのみ）
		float uvAmount = 1.0f / static_cast<float>(MAX_POLYGON - 3);

		for (int i = 0; i < MAX_POLYGON - 3; ++i)
		{
			v += uvAmount;

			AddVertex(trailDatas[i + 1].tail_, { 0.0f,v }, trailDatas[i + 1].color_);
			AddVertex(trailDatas[i + 1].head_, { 1.0f,v }, trailDatas[i + 1].color_);

			float partitionV = v + uvAmount / static_cast<float>(tMaxNum);
			for (int partitionIndex = 0; partitionIndex < tMaxNum; ++partitionIndex)
			{
				DirectX::XMFLOAT3 pos0 = {};
				DirectX::XMStoreFloat3(&pos0, DirectX::XMVectorCatmullRom(
					DirectX::XMLoadFloat3(&trailDatas[i].tail_),
					DirectX::XMLoadFloat3(&trailDatas[i + 1].tail_),
					DirectX::XMLoadFloat3(&trailDatas[i + 2].tail_),
					DirectX::XMLoadFloat3(&trailDatas[i + 3].tail_),
					t[partitionIndex]));

				DirectX::XMFLOAT3 pos1 = {};
				DirectX::XMStoreFloat3(&pos1, DirectX::XMVectorCatmullRom(
					DirectX::XMLoadFloat3(&trailDatas[i].head_),
					DirectX::XMLoadFloat3(&trailDatas[i + 1].head_),
					DirectX::XMLoadFloat3(&trailDatas[i + 2].head_),
					DirectX::XMLoadFloat3(&trailDatas[i + 3].head_),
					t[partitionIndex]));

				//色の算出（とりあえず平均でやってみる）
				Vector4 color;
				for (int j = 0; j < 4; j++)
				{
					color += trailDatas[i + j].color_;
				}
				color /= 4;

				AddVertex(pos0, { 0.0f,partitionV },color);
				AddVertex(pos1, { 1.0f,partitionV },color);

				partitionV += uvAmount / static_cast<float>(tMaxNum);
			}

		}
	}
}

void AbyssEngine::SwordTrailRenderer::Render()
{
	const auto& deviceContext = DXSystem::GetDeviceContext();

	//ゲープロパクってみる
	// シェーダー設定
	deviceContext->VSSetShader(vertexShader_.Get(), nullptr, 0);
	deviceContext->PSSetShader(pixelShader_.Get(), nullptr, 0);
	deviceContext->IASetInputLayout(inputLayout_.Get());

	// 定数バッファ設定
	constantBuffer_->Activate(0, CBufferUsage::p);

	// 頂点バッファ設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);

	//テクスチャ
	texture_->Set(1, Shader_Type::Pixel);

	// 描画
	UINT totalVertexCount = static_cast<UINT>(vertices_.size());
	UINT start = 0;
	UINT count = (totalVertexCount < VertexCapacity) ? totalVertexCount : VertexCapacity;

	while (start < totalVertexCount)
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		HRESULT hr = deviceContext->Map(vertexBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		memcpy(mappedSubresource.pData, &vertices_[start], sizeof(Vertex) * count);

		deviceContext->Unmap(vertexBuffer_.Get(), 0);

		deviceContext->Draw(count, 0);

		start += count;
		if ((start + count) > totalVertexCount)
		{
			count = totalVertexCount - start;
		}
	}

	vertices_.clear();
}

void AbyssEngine::SwordTrailRenderer::RecalculateFrame()
{
	//更新
	Update();
}

void AbyssEngine::SwordTrailRenderer::DrawImGui()
{
	if (ImGui::TreeNode("Sword Trail Renderer"))
	{
		ImGui::Checkbox("Active Trail", &activeTrail_);

		ImGui::DragFloat("Sword Range", &swordRange_, 0.1f);
		ImGui::DragFloat3("Offset Pos", &offsetPosition_.x, 0.05f);
		ImGui::DragFloat3("Offset Rot", &offsetRotation_.x, 0.5f);

		ImGui::ColorEdit4("Color", &color_.x,ImGuiColorEditFlags_PickerHueWheel);
		ImGui::DragFloat("Intensity", &constantBuffer_->data_.intensity_, 0.1f);

		ImGui::TreePop();
	}
}

void AbyssEngine::SwordTrailRenderer::AddVertex(Vector3 pos, Vector2 uv, Vector4 color)
{
	Vertex v;
	v.position_ = pos;
	v.texcoord_ = uv;
	v.color_ = color;
	vertices_.emplace_back(v);
}
