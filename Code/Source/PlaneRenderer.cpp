#include "PlaneRenderer.h"
#include "Actor.h"
#include "Texture.h"
#include "imgui/imgui.h"
#include "Engine.h"
#include "RenderManager.h"

using namespace AbyssEngine;

void PlaneRenderer::Initialize(const std::shared_ptr<Actor>& actor)
{
    //初期化
    actor_ = actor;
    transform_ = actor->GetTransform();

    //マネージャーに登録
    canRender_ = true;
    SetActive(GetEnabled());

    // 頂点データ
    Vertex vertices[] = {
        { Vector3(-1.0f, -1.0f, 0.0f),Vector2(0.0f, 1.0f) },
        { Vector3(-1.0f,  1.0f, 0.0f),Vector2(0.0f, 0.0f) },
        { Vector3(1.0f,  1.0f, 0.0f), Vector2(1.0f, 0.0f) },
        { Vector3(1.0f, -1.0f, 0.0f), Vector2(1.0f, 1.0f) }
    };

    // インデックスデータ
    unsigned int indices[] = 
    {
        0, 1, 2,
        0, 2, 3
    };

    // 頂点バッファの説明
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    // 頂点バッファの初期データ
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;

    HRESULT hr = { S_OK };

    // 頂点バッファの作成
    hr = DXSystem::GetDevice()->CreateBuffer(&bd, &initData, vertexBuffer_.GetAddressOf());

    // インデックスバッファの設定
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(indices);
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    // インデックスバッファの初期データ
    initData.pSysMem = indices;

    // インデックスバッファの作成
    hr = DXSystem::GetDevice()->CreateBuffer(&bd, &initData, indexBuffer_.GetAddressOf());

    //テクスチャの読み込み
    texture_ = Texture::Load(filePath_);

    //シェーダー読み込み
    D3D11_INPUT_ELEMENT_DESC inputLayout[] = 
    {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(inputLayout);

    vertexShader_ = Shader<ID3D11VertexShader>::Emplace("./Resources/Shader/PlaneVS.cso", inputLayout_.GetAddressOf(), inputLayout, numElements);
    pixelShader_  = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/PlanePS.cso");

    //定数バッファ生成
    constantBuffer_ = std::make_unique<ConstantBuffer<Constants>>();
}

void PlaneRenderer::RecalculateFrame()
{
    //行列更新
    constantBuffer_->data_.worldMatrix_ = transform_->CalcWorldMatrix();
}

void PlaneRenderer::Render()
{
    if (!canRender_)return;

    if (!visibility_)return;

    DXSystem::SetBlendState(BS_State::Alpha);

    //定数バッファ更新
    constantBuffer_->Activate(0, CBufferUsage::vp);

    const auto& context = DXSystem::GetDeviceContext();

    // 入力アセンブラーの設定
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(indexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // シェーダーリソースとサンプラーステートの設定
    texture_->Set(0, Shader_Type::Pixel);

    // シェーダーの設定
    context->VSSetShader(vertexShader_.Get(), nullptr, 0);
    context->PSSetShader(pixelShader_.Get(), nullptr, 0);

    // 描画
    context->DrawIndexed(6, 0, 0);
}

bool PlaneRenderer::DrawImGui()
{
#if _DEBUG
    if (ImGui::TreeNode("PlaneRenderer"))
    {
        char path[256]{};
        if (path == 0) { strncpy_s(path, filePath_.c_str(), sizeof(path) - 1); }
        ImGui::InputText("FilePath", path,sizeof(path));
        if (ImGui::Button("Set Texture"))
        {
            texture_ = Texture::Load(path);
        }

        ImGui::Image(texture_->GetResource().Get(), { 256,256 });

        ImGui::InputFloat2("Crop Size", &cropSize_.x);
        ImGui::InputFloat2("Pivot", &pivot_.x);

        ImGui::ColorPicker4("Color", &constantBuffer_->data_.color_.x, ImGuiColorEditFlags_PickerHueWheel);

        ImGui::TreePop();
    }
#endif // _DEBUG
    return true;
}

void AbyssEngine::PlaneRenderer::SetActive(bool value)
{
    if (value)
    {
        if (!isCalled_)
        {
            Engine::renderManager_->Add(static_pointer_cast<PlaneRenderer>(shared_from_this()));
            isCalled_ = true;
        }
    }
}
