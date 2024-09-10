#include "SpriteRenderer.h"
#include "Texture.h"
#include "Transform.h"
#include "Engine.h"
#include "RenderManager.h"
#include "Actor.h"
#include "imgui/imgui.h"

using namespace std;
using namespace AbyssEngine;
using namespace DirectX;

void SpriteRenderer::Initialize(const shared_ptr<Actor>& actor)
{
	//マネージャーの登録と初期化
	actor_ = actor;
	transform_ = actor->GetTransform();

    const Vertex v[] = {
        Vector3(-0.5f, 0.5f,0),	Vector2(0,0),Vector4(1,1,1,1), //左上
        Vector3( 0.5f, 0.5f,0),	Vector2(1,0),Vector4(1,1,1,1), //右上
        Vector3(-0.5f,-0.5f,0),	Vector2(0,1),Vector4(1,1,1,1), //左下
        Vector3( 0.5f,-0.5f,0),	Vector2(1,1),Vector4(1,1,1,1), //右下
    };

    //頂点バッファ作成
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = sizeof(v);
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA res;
    ZeroMemory(&res, sizeof(res));
    res.pSysMem = v;
	res.SysMemPitch = 0;
	res.SysMemSlicePitch = 0;
    auto hr = DXSystem::GetDevice()->CreateBuffer(&bd, &res, vertexBuffer_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	vertexShader_ = Shader<ID3D11VertexShader>::Emplace("./Resources/Shader/SpriteVS.cso", inputLayout_.GetAddressOf(),inputElementDesc, ARRAYSIZE(inputElementDesc));
	pixelShader_ = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/SpritePS.cso");

    //テクスチャ読み込み
    if (filePath_.empty())
    {
        _ASSERT_EXPR(false, L"ファイルパスが空です");
    }
    else
    {
        texture_ = Texture::Load(filePath_);
    }

	//初期値は元画像のサイズを設定しておく
	size_ = Vector2(static_cast<float>(texture_->GetWidth()), static_cast<float>(texture_->GetHeight()));
	uvSize_ = size_;

	canRender_ = true;

	SetActive(GetEnabled());
}

void SpriteRenderer::Render()
{
    if (!canRender_)return;

    //非表示の場合処理しない
    if (!visibility_)return;

    //頂点バッファの指定
    constexpr UINT stride = sizeof(Vertex);
    constexpr UINT offset = 0;
    DXSystem::GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);

	DXSystem::GetDeviceContext()->IASetInputLayout(inputLayout_.Get());

    //テクスチャの設定
    texture_->Set(1, Shader_Type::Pixel);

	DXSystem::GetDeviceContext()->VSSetShader(vertexShader_.Get(), nullptr, 0);
	DXSystem::GetDeviceContext()->PSSetShader(pixelShader_.Get(), nullptr, 0);

    //描画
    DXSystem::GetDeviceContext()->Draw(4, 0);
}

void SpriteRenderer::RecalculateFrame()
{
    if (!canRender_)return;

    //表示切替
    if (!visibility_)return;

    //頂点データ設定
    Vertex data_[4];

    const Vector3 transPos = actor_->GetTransform()->GetPosition() + offsetPos_;
    const Vector3 transScale = actor_->GetTransform()->GetScale();
    const Vector2 scaledSize = size_ * Vector2(transScale.x,transScale.y);

    data_[0].pos_.x = transPos.x;
    data_[0].pos_.y = transPos.y;
    data_[0].pos_.z = 0.0f;

    data_[1].pos_.x = transPos.x + scaledSize.x;
    data_[1].pos_.y = transPos.y;
    data_[1].pos_.z = 0.0f;

    data_[2].pos_.x = transPos.x;
    data_[2].pos_.y = transPos.y + scaledSize.y;
    data_[2].pos_.z = 0.0f;

    data_[3].pos_.x = transPos.x + scaledSize.x;
    data_[3].pos_.y = transPos.y + scaledSize.y;
    data_[3].pos_.z = 0.0f;

	//中心点に合わせて移動
	/*for (auto& d : data_)
	{
		d.pos_.x -= pivot_.x;
		d.pos_.y -= pivot_.y;
	}*/

	// 中心座標を原点へ
	const float mx = transPos.x + scaledSize.x * 0.5f;
	const float my = transPos.y + scaledSize.y * 0.5f;
	data_[0].pos_.x -= mx; data_[0].pos_.y -= my;
	data_[1].pos_.x -= mx; data_[1].pos_.y -= my;
	data_[2].pos_.x -= mx; data_[2].pos_.y -= my;
	data_[3].pos_.x -= mx; data_[3].pos_.y -= my;

	const float z = transform_->GetEulerAngles().z;
	const float cos = cosf(XMConvertToRadians(z));
	const float sin = sinf(XMConvertToRadians(z));

	float rx = data_[0].pos_.x;
	float ry = data_[0].pos_.y;
	data_[0].pos_.x = cos * rx + -sin * ry;
	data_[0].pos_.y = sin * rx + cos * ry;

	rx = data_[1].pos_.x;
	ry = data_[1].pos_.y;
	data_[1].pos_.x = cos * rx + -sin * ry;
	data_[1].pos_.y = sin * rx + cos * ry;

	rx = data_[2].pos_.x;
	ry = data_[2].pos_.y;
	data_[2].pos_.x = cos * rx + -sin * ry;
	data_[2].pos_.y = sin * rx + cos * ry;

	rx = data_[3].pos_.x;
	ry = data_[3].pos_.y;
	data_[3].pos_.x = cos * rx + -sin * ry;
	data_[3].pos_.y = sin * rx + cos * ry;

	// 動かした分戻す
	data_[0].pos_.x += mx; data_[0].pos_.y += my;
	data_[1].pos_.x += mx; data_[1].pos_.y += my;
	data_[2].pos_.x += mx; data_[2].pos_.y += my;
	data_[3].pos_.x += mx; data_[3].pos_.y += my;

	// 正規化デバイス座標系
	for (auto& i : data_)
	{
		i.pos_.x = 2.0f * i.pos_.x / DXSystem::GetScreenWidth() - 1.0f;
		i.pos_.y = 1.0f - 2.0f * i.pos_.y / DXSystem::GetScreenHeight();
		i.pos_.z = 0.0f;
	}

	//テクスチャ座標設定
	data_[0].tex_.x = uvOrigin_.x;
	data_[0].tex_.y = uvOrigin_.y;
	data_[1].tex_.x = uvOrigin_.x + uvSize_.x;
	data_[1].tex_.y = uvOrigin_.y;
	data_[2].tex_.x = uvOrigin_.x;
	data_[2].tex_.y = uvOrigin_.y + uvSize_.y;
	data_[3].tex_.x = uvOrigin_.x + uvSize_.x;
	data_[3].tex_.y = uvOrigin_.y + uvSize_.y;

	/*data[0].tex_.x = 0.0f;
	data[0].tex_.y = 0.0f;
	data[1].tex_.x = 1.0f;
	data[1].tex_.y = 0.0f;
	data[2].tex_.x = 0.0f;
	data[2].tex_.y = 1.0f;
	data[3].tex_.x = 1.0f;
	data[3].tex_.y = 1.0f;*/

	//UV座標
	const float w = static_cast<float>(texture_->GetWidth());
	const float h = static_cast<float>(texture_->GetHeight());
	for (auto& i : data_)
	{
		i.tex_.x = i.tex_.x / w;
		i.tex_.y = i.tex_.y / h;
	}

	//頂点カラー
	data_[0].color_ = color_;
	data_[1].color_ = color_;
	data_[2].color_ = color_;
	data_[3].color_ = color_;

	constexpr UINT subresourceIndex = 0;
	D3D11_MAPPED_SUBRESOURCE mapped;
	const auto hr = DXSystem::GetDeviceContext()->Map(vertexBuffer_.Get(), subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (SUCCEEDED(hr))
	{
		memcpy(mapped.pData, data_, sizeof(data_));
		DXSystem::GetDeviceContext()->Unmap(vertexBuffer_.Get(), subresourceIndex);
	}
}

void AbyssEngine::SpriteRenderer::DrawImGui()
{
	if (ImGui::TreeNode("SpriteRenderer"))
	{
		ImGui::DragFloat2("Size", &size_.x, 0.1f, 0.0f);
		ImGui::DragFloat2("UV Origin", &uvOrigin_.x, 0.1f, 0.0f);
		ImGui::DragFloat2("UV Size", &uvSize_.x, 0.1f, 0.0f);
		ImGui::DragFloat2("Pivot", &pivot_.x, 1, 0.0f);
		ImGui::DragFloat2("Offset Pos", &offsetPos_.x, 0.1f);

		ImGui::ColorPicker4("Color",&color_.x);

		ImGui::TreePop();
	}
}

void SpriteRenderer::SetActive(const bool value)
{
	if (value)
	{
		if (!isCalled_)
		{
			Engine::renderManager_->Add(static_pointer_cast<SpriteRenderer>(shared_from_this()));
			isCalled_ = true;
		}
	}
}

bool SpriteRenderer::FadeIn(float alpha, float changeSpeed)
{
	changeSpeed *= Time::deltaTime_;

	if (color_.w + changeSpeed >= alpha)
	{
		color_.w = alpha;
		return true;
	}

	color_.w += changeSpeed;
	return false;
}

bool SpriteRenderer::FadeOut(float alpha, float changeSpeed)
{
	changeSpeed *= Time::deltaTime_;

	if (color_.w - changeSpeed <= alpha)
	{
		color_.w = alpha;
		return true;
	}

	color_.w -= changeSpeed;
	return false;
}
