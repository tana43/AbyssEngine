#include "PlaneRenderer.h"
#include "Actor.h"
#include "Texture.h"
#include "imgui/imgui.h"
#include "Engine.h"
#include "RenderManager.h"

using namespace AbyssEngine;

void PlaneRenderer::Initialize(const std::shared_ptr<Actor>& actor)
{
    //������
    actor_ = actor;
    transform_ = actor->GetTransform();

    //�}�l�[�W���[�ɓo�^
    canRender_ = true;
    SetActive(GetEnabled());

    // ���_�f�[�^
    Vertex vertices[] = {
        { Vector3(-1.0f, -1.0f, 0.0f),Vector2(0.0f, 1.0f) },
        { Vector3(-1.0f,  1.0f, 0.0f),Vector2(0.0f, 0.0f) },
        { Vector3(1.0f,  1.0f, 0.0f), Vector2(1.0f, 0.0f) },
        { Vector3(1.0f, -1.0f, 0.0f), Vector2(1.0f, 1.0f) }
    };

    // �C���f�b�N�X�f�[�^
    unsigned int indices[] = 
    {
        0, 1, 2,
        0, 2, 3
    };

    // ���_�o�b�t�@�̐���
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    // ���_�o�b�t�@�̏����f�[�^
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;

    HRESULT hr = { S_OK };

    // ���_�o�b�t�@�̍쐬
    hr = DXSystem::GetDevice()->CreateBuffer(&bd, &initData, vertexBuffer_.GetAddressOf());

    // �C���f�b�N�X�o�b�t�@�̐ݒ�
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(indices);
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    // �C���f�b�N�X�o�b�t�@�̏����f�[�^
    initData.pSysMem = indices;

    // �C���f�b�N�X�o�b�t�@�̍쐬
    hr = DXSystem::GetDevice()->CreateBuffer(&bd, &initData, indexBuffer_.GetAddressOf());

    //�e�N�X�`���̓ǂݍ���
    texture_ = Texture::Load(filePath_);

    //�V�F�[�_�[�ǂݍ���
    D3D11_INPUT_ELEMENT_DESC inputLayout[] = 
    {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(inputLayout);

    vertexShader_ = Shader<ID3D11VertexShader>::Emplace("./Resources/Shader/PlaneVS.cso", inputLayout_.GetAddressOf(), inputLayout, numElements);
    pixelShader_  = Shader<ID3D11PixelShader>::Emplace("./Resources/Shader/PlanePS.cso");

    //�萔�o�b�t�@����
    constantBuffer_ = std::make_unique<ConstantBuffer<Constants>>();
}

void PlaneRenderer::RecalculateFrame()
{
    //�s��X�V
    constantBuffer_->data_.worldMatrix_ = transform_->CalcWorldMatrix();
}

void PlaneRenderer::Render()
{
    if (!canRender_)return;

    if (!visibility_)return;

    DXSystem::SetBlendState(BS_State::Alpha);

    //�萔�o�b�t�@�X�V
    constantBuffer_->Activate(0, CBufferUsage::vp);

    const auto& context = DXSystem::GetDeviceContext();

    // ���̓A�Z���u���[�̐ݒ�
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(indexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // �V�F�[�_�[���\�[�X�ƃT���v���[�X�e�[�g�̐ݒ�
    texture_->Set(0, Shader_Type::Pixel);

    // �V�F�[�_�[�̐ݒ�
    context->VSSetShader(vertexShader_.Get(), nullptr, 0);
    context->PSSetShader(pixelShader_.Get(), nullptr, 0);

    // �`��
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
