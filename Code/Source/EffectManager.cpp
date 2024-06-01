#include "DxSystem.h"
#include "EffectManager.h"

using namespace AbyssEngine;

void EffectManager::Initialize()
{
    //Effekseerレンダラ生成
    effekseerRenderer = EffekseerRendererDX11::Renderer::Create(DXSystem::GetDevice(),
        DXSystem::GetDeviceContext(), 2048);

    //Effekseerマネージャー生成
    effekseerManager = Effekseer::Manager::Create(2048);

    //Effekseerレンダラの各種設定（特別なカスタマイズをしない場合は定型的に以下の設定でOK）
#define SET_RENDERER(renderer) effekseerManager->Set##renderer(effekseerRenderer->Create##renderer())
    SET_RENDERER(SpriteRenderer);
    SET_RENDERER(RibbonRenderer);
    SET_RENDERER(RingRenderer);
    SET_RENDERER(TrackRenderer);
    SET_RENDERER(ModelRenderer);
    //Effekseer内でのローダーの設定（特別なカスタマイズをしない場合は以下の設定でOK）
#define SET_LOADER(loader) effekseerManager->Set##loader(effekseerRenderer->Create##loader())
    SET_LOADER(TextureLoader);
    SET_LOADER(ModelLoader);
    SET_LOADER(MaterialLoader);

    //Effecseerを左手座標系で計算する
    effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

void EffectManager::Finalize()
{
    //EffekseerManagerなどはスマートポイントによって破棄されるので何もしない
}

void EffectManager::Update(float elapsedTime)
{
    //エフェクト更新処理（引数にはフレームの経過時間を渡す）
    effekseerManager->Update(elapsedTime * 60.0f);
}

void EffectManager::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    //ビュー＆プロジェクション行列をEffekseerレンダラに設定
    effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
    effekseerRenderer->SetProjectionMatrix(
        *reinterpret_cast<const Effekseer::Matrix44*>(&projection));

    //Effekseer描画開始
    effekseerRenderer->BeginRendering();

    //Effekseer描画実行
    //マネージャー単位で描画するので描画順を制御する場合はマネージャーを複数個作成し、
    //Draw()関数を実行する順序で出来そう
    effekseerManager->Draw();

    //Effekseer描画終了
    effekseerRenderer->EndRendering();
}
