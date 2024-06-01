#include "DxSystem.h"
#include "EffectManager.h"

using namespace AbyssEngine;

void EffectManager::Initialize()
{
    //Effekseer�����_������
    effekseerRenderer = EffekseerRendererDX11::Renderer::Create(DXSystem::GetDevice(),
        DXSystem::GetDeviceContext(), 2048);

    //Effekseer�}�l�[�W���[����
    effekseerManager = Effekseer::Manager::Create(2048);

    //Effekseer�����_���̊e��ݒ�i���ʂȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͒�^�I�Ɉȉ��̐ݒ��OK�j
#define SET_RENDERER(renderer) effekseerManager->Set##renderer(effekseerRenderer->Create##renderer())
    SET_RENDERER(SpriteRenderer);
    SET_RENDERER(RibbonRenderer);
    SET_RENDERER(RingRenderer);
    SET_RENDERER(TrackRenderer);
    SET_RENDERER(ModelRenderer);
    //Effekseer���ł̃��[�_�[�̐ݒ�i���ʂȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͈ȉ��̐ݒ��OK�j
#define SET_LOADER(loader) effekseerManager->Set##loader(effekseerRenderer->Create##loader())
    SET_LOADER(TextureLoader);
    SET_LOADER(ModelLoader);
    SET_LOADER(MaterialLoader);

    //Effecseer��������W�n�Ōv�Z����
    effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

void EffectManager::Finalize()
{
    //EffekseerManager�Ȃǂ̓X�}�[�g�|�C���g�ɂ���Ĕj�������̂ŉ������Ȃ�
}

void EffectManager::Update(float elapsedTime)
{
    //�G�t�F�N�g�X�V�����i�����ɂ̓t���[���̌o�ߎ��Ԃ�n���j
    effekseerManager->Update(elapsedTime * 60.0f);
}

void EffectManager::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    //�r���[���v���W�F�N�V�����s���Effekseer�����_���ɐݒ�
    effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
    effekseerRenderer->SetProjectionMatrix(
        *reinterpret_cast<const Effekseer::Matrix44*>(&projection));

    //Effekseer�`��J�n
    effekseerRenderer->BeginRendering();

    //Effekseer�`����s
    //�}�l�[�W���[�P�ʂŕ`�悷��̂ŕ`�揇�𐧌䂷��ꍇ�̓}�l�[�W���[�𕡐��쐬���A
    //Draw()�֐������s���鏇���ŏo������
    effekseerManager->Draw();

    //Effekseer�`��I��
    effekseerRenderer->EndRendering();
}
