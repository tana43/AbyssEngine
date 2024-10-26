#include "TrailRenderer.h"
#include "Actor.h"
#include "DxSystem.h"
#include "Shader.h"
#include "Misc.h"
#include "Texture.h"
#include "Engine.h"
#include "RenderManager.h"
#include "Camera.h"
#include "imgui/imgui.h"

using namespace AbyssEngine;

void TrailRenderer::Initialize(const std::shared_ptr<Actor>& actor)
{
    actor_ = actor;
    transform_ = actor->GetTransform();

	//マネージャーに登録
	Engine::renderManager_->Add(std::static_pointer_cast<TrailRenderer>(shared_from_this()));

	//　初期化
	{
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		// 頂点シェーダー
		vertexShader_ = Shader<ID3D11VertexShader>::Emplace(
			"./Resources/Shader/TrailRendererVS.cso", 
			inputLayout_.ReleaseAndGetAddressOf(),
			inputElementDesc,
			_countof(inputElementDesc));

		// ピクセルシェーダー
		pixelShader_ = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/TrailRendererPS.cso");

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

void TrailRenderer::Update()
{
	//　保存していた頂点バッファを１フレーム分ずらす
	{
		for (int x = MAX_POLYGON - 1; x > -1; --x)
		{
			const int previousIndex = x - 1; 
			if (previousIndex == -1)
			{
				trailDatas[0] = { {0,0,0},{0,0,0} };
			}
			else
			{
				trailDatas[x] = trailDatas[previousIndex];
			}
		}
	}

	//　アクターの座標から頂点を作成し、保存する
	{
		const Vector3 pos = transform_->GetPosition() + offsetPosition_;
		trailDatas[0].position_ = pos;

		//前回と今回の座標から進行方向のベクトルを算出
		const Vector3 dir = DirectX::XMVector3Normalize(pos - previousPosition_);
		if (dir.LengthSquared() > 0.01f)trailDatas[0].moveDirection_ = dir;
		else trailDatas[0].moveDirection_ = trailDatas[1].moveDirection_;
		previousPosition_ = pos;
	}

	//ポリゴン作成
	{
#if 1
		// 保存していた頂点バッファでポリゴンを作る
		for (int i = 0; i < MAX_POLYGON; ++i)
		{
			//UV値の加算量
			float amount = 1.0f / (ARRAYSIZE(trailDatas) - 1);
			float u = 0;

			//幅を減らしていく
			float weight = static_cast<float>(MAX_POLYGON - i) / static_cast<float>(MAX_POLYGON);
			weight *= weight;
			float width = width_ * weight;

			CreatePolygon(trailDatas[i], u,width);
			u += amount;
		}
#else
		//スプライン補間をしつつトレイル作成
		static const int tMaxNum = 7;
		const float t[tMaxNum] = { 0.125f, 0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.875f };
		

		CreatePolygon(trailDatas[0], 0,width_);

		for (int i = 0; i < MAX_POLYGON - 3; ++i)
		{
			float amount = 1.0f / (ARRAYSIZE(trailDatas) - 1);
			float u = 0;

			//幅を減らしていく
			float weight = static_cast<float>(MAX_POLYGON - i) / static_cast<float>(MAX_POLYGON);
			weight *= weight;
			float width = width_ * weight;

			CreatePolygon(trailDatas[i + 1],u, width);

			for (int partitionIndex = 0; partitionIndex < tMaxNum; ++partitionIndex)
			{
				Vector3 pos = {};
				pos = DirectX::XMVectorCatmullRom(
					DirectX::XMLoadFloat3(&trailDatas[i].position_),
					DirectX::XMLoadFloat3(&trailDatas[i + 1].position_),
					DirectX::XMLoadFloat3(&trailDatas[i + 2].position_),
					DirectX::XMLoadFloat3(&trailDatas[i + 3].position_),
					t[partitionIndex]);
				TrailData t;
				t.position_ = pos;
				t.moveDirection_ = DirectX::XMVector3Normalize(trailDatas[i + 3].position_ - trailDatas[i].position_);
				CreatePolygon(t, u, width);

				u += amount;
			}
		}
#endif // 0

		
	}
}

void TrailRenderer::Render()
{
	const auto& deviceContext = DXSystem::GetDeviceContext();
    //UV値は１から減らしてスクロールしていく

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

void TrailRenderer::RecalculateFrame()
{
	Update();
}

void AbyssEngine::TrailRenderer::DrawImGui()
{
	if (ImGui::TreeNode("TrailRenderer"))
	{
		ImGui::DragFloat("Trail Width",&width_, 0.01f, 0.0f);
		ImGui::ColorEdit4("Color", &constantBuffer_->data_.color_.x, ImGuiColorEditFlags_PickerHueWheel);

		ImGui::TreePop();
	}
}

void TrailRenderer::AddVertex(const Vector3& position, const Vector2& uv, const Vector4& color)
{
	Vertex v;
	v.position_ = position;
	v.color_ = color;
	v.texcoord_ = uv;
	vertices_.emplace_back(v);
}

void AbyssEngine::TrailRenderer::CreatePolygon(const TrailData& trailData,const float& texcoordX,const float& width)
{
	const Vector4 color = { 1,1,1,1 };

	//カメラの向きから頂点を作成する
	const auto& camera = Camera::GetMainCamera();
	const Vector3 cameraEye = camera->GetEye();
	const Vector3 cameraFocus = camera->GetFocus();

	const Vector3 cameraForward = cameraFocus - cameraEye;
	const Vector3 direCamera = DirectX::XMVector3Normalize(cameraForward);
	const Vector3 cross = direCamera.Cross(trailData.moveDirection_);
	const Vector3 displace = cross * width;
	//const Vector3 displace = camera->GetTransform()->GetUp() * width;
	Vector3 positions[2] = {
		trailData.position_ + displace,
		trailData.position_ - displace,
	};

	//if (positions[0].y < positions[1].y)
	//{
	//	AddVertex(positions[1], {texcoordX,0.0f}, color);
	//	AddVertex(positions[0], {texcoordX,1.0f}, color);
	//}
	//else

	{
		AddVertex(positions[0], {texcoordX,1.0f}, color);
		AddVertex(positions[1], {texcoordX,0.0f}, color);
	}
}
