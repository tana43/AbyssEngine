#include <initguid.h>
#include "DXSystem.h"

#include <DirectXMath.h>
#include <dxgidebug.h>
#include "Misc.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace AbyssEngine;

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dcompiler.lib" )


DXSystem*           DXSystem::instance = nullptr;
HWND				DXSystem::hwnd_;
DXGI_SAMPLE_DESC	DXSystem::MSAA_;
int DXSystem::screenWidth_  = 1920;
//int DXSystem::screenWidth_  = 3840;
int DXSystem::screenHeight_ = 1080;
//int DXSystem::screenHeight_ = 2160;

DXSystem::DXSystem()
{
    _ASSERT_EXPR(instance == nullptr,"DXSystemはすでに生成されています");
    instance = this;
}

AbyssEngine::DXSystem::~DXSystem()
{
    //instance->deviceContext_->Flush();
}

bool DXSystem::Initialize(HWND hWnd, int width, int height_)
{
    hwnd_ = hWnd;
    CreateDevice();
    InitializeRenderTarget();
    CreateRasterizerState();
    CreateBlendState();

    return true;
}

void DXSystem::Release()
{
    instance->deviceContext_->ClearState();
    ReportLiveObjectsWrapper();
}

void DXSystem::Clear()
{
    auto& deviceContext = instance->deviceContext_;
    auto& renderTargetView = instance->renderTargetView_;
    auto& depthStencilView = instance->depthStencilView_;

    //レンダーターゲットビュー設定
    deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

    float clearColor[4] = { 0.4f,0.4f,0.4f,1 };
    deviceContext->ClearRenderTargetView(renderTargetView.Get(), clearColor);
    deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DXSystem::Flip(int n)
{
    //フリップ処理
    instance->swapChain_->Present(n, 0);

    auto hr = instance->device_->GetDeviceRemovedReason();
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}

void DXSystem::SetViewport(int width, int height_, int num)
{
    D3D11_VIEWPORT vp;
    vp.Width = static_cast<FLOAT>(width);
    vp.Height = static_cast<FLOAT>(height_);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    instance->deviceContext_->RSSetViewports(num, &vp);
}

void DXSystem::SetDefaultView()
{
    instance->deviceContext_->OMSetRenderTargets(1, instance->renderTargetView_.GetAddressOf(), instance->depthStencilView_.Get());
    SetViewport(screenWidth_, screenHeight_);
}

void DXSystem::SetDepthStencilState(DS_State type,UINT stencilRef)
{
    instance->deviceContext_->OMSetDepthStencilState(instance->depthStencilStates_[static_cast<int>(type)].Get(),stencilRef);
}

void DXSystem::SetRasterizerState(RS_State type)
{
    instance->deviceContext_->RSSetState(instance->rasterizerStates_[static_cast<int>(type)].Get());
}

void AbyssEngine::DXSystem::SetBlendState(BS_State type)
{
    instance->deviceContext_->OMSetBlendState(instance->blendStates_[static_cast<int>(type)].Get(),nullptr,0xFFFFFFFF);
}

HRESULT DXSystem::CreateDevice()
{
    HRESULT hr;
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if _DEBUG
    //デバッグレイヤー設定
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    //デバイス生成
    hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        &featureLevel,
        1,
        D3D11_SDK_VERSION,
        instance->device_.GetAddressOf(),
        nullptr,
        instance->deviceContext_.GetAddressOf());

    if (FAILED(hr))
    {
        MessageBoxW(hwnd_, L"D3D11CreateDevice", L"Err", MB_ICONSTOP); 
        return S_FALSE;
    }

#if _DEBUG
    //デバッグモード開始
    //if (dxgiDebug_ == nullptr)
    //{
    //    //作成
    //    typedef HRESULT(__stdcall* fPtr)(const IID&, void**);
    //    HMODULE hDll = GetModuleHandleW(L"dxgidebug.dll");
    //    fPtr DXGIGetDebugInterface = (fPtr)GetProcAddress(hDll, "DXGIGetDebugInterface");

    //    DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&dxgiDebug_);

    //    //出力
    //    dxgiDebug_->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
    //}
    //else
    //{
    //    dxgiDebug_->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
    //}
#endif // _DEBUG

    //使用可能なMSAAを取得
    constexpr int maxCount = 4;//D3D11_MAX_MULTISAMPLE_COUNT;
    for (int i = 0; i < maxCount; i++)
    {
        UINT Quality;
        if SUCCEEDED(instance->device_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM,i,&Quality))
        {
            if (0 < Quality)
            {
                MSAA_.Count = i;
                MSAA_.Quality = Quality - 1;
            }
        }
    }

    //インターフェース対応
    Microsoft::WRL::ComPtr<IDXGIDevice1> hpDXGI;
    if (FAILED(instance->device_.Get()->QueryInterface(__uuidof(IDXGIDevice1), reinterpret_cast<void**>(hpDXGI.GetAddressOf()))))
    {
        MessageBoxW(hwnd_, L"GetAdaper", L"Err", MB_ICONSTOP);
        return S_FALSE;
    }

    //アダプター取得
    Microsoft::WRL::ComPtr<IDXGIAdapter> hpAdapter;
    if (FAILED(hpDXGI->GetAdapter(hpAdapter.GetAddressOf())))
    {
        MessageBoxW(hwnd_, L"GetAdapter", L"Err", MB_ICONSTOP);
        return S_FALSE;
    }

    //ファクトリー取得
    Microsoft::WRL::ComPtr<IDXGIFactory> hpDXGIFactory;
    if (FAILED(hpAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(hpDXGIFactory.GetAddressOf()))))
    {
        MessageBoxW(hwnd_, L"GetParent", L"Err", MB_ICONSTOP);
        return S_FALSE;
    }

    RECT clientRect;
    GetClientRect(hwnd_, &clientRect);
    screenWidth_ = clientRect.right;
    screenHeight_ = clientRect.bottom;

    //スワップチェイン作成
    DXGI_SWAP_CHAIN_DESC scd;
    scd.BufferDesc.Width = screenWidth_;
    scd.BufferDesc.Height = screenHeight_;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    scd.SampleDesc = MSAA_;
    //scd.SampleDesc.Count = 1;
    //scd.SampleDesc.Quality = 0;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 1;
    scd.OutputWindow = hwnd_;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    if (FAILED(hpDXGIFactory->CreateSwapChain(instance->device_.Get(), &scd, instance->swapChain_.GetAddressOf())))
    {
        MessageBoxW(hwnd_, L"CreateSwapChain", L"Err", MB_ICONSTOP);
        return S_FALSE;
    }

    return S_OK;
}

bool DXSystem::CreateDepthStencil()
{
    auto& device = instance->device_;
    auto& depthStencilTexture = instance->depthStencilTexture_;
    auto& depthStencilView = instance->depthStencilView_;
    auto& depthStencilStates = instance->depthStencilStates_;

#if 1
    //深度ステンシル設定
    D3D11_TEXTURE2D_DESC td;
    ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));//変数の中身を０クリアするマクロ
    td.Width = screenWidth_;
    td.Height = screenHeight_;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R32G8X24_TYPELESS;
    td.SampleDesc = MSAA_;
    //td.SampleDesc.Count = 1;
    //td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    td.CPUAccessFlags = 0;
    td.MiscFlags = 0;

    //深度ステンシルテクスチャ生成
    HRESULT hr = device->CreateTexture2D(&td, nullptr, &depthStencilTexture);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    //深度ステンシルビュー設定
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
    ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    dsvd.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    dsvd.Texture2D.MipSlice = 0;

    //深度ステンシルビュー生成
    hr = device->CreateDepthStencilView(depthStencilTexture.Get(), &dsvd, depthStencilView.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
#else
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer{};
    D3D11_TEXTURE2D_DESC texture2dDesc{};
    texture2dDesc.Width = screenWidth_;
    texture2dDesc.Height = screenHeight_;
    texture2dDesc.MipLevels = 1;
    texture2dDesc.ArraySize = 1;
    texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    //texture2dDesc.SampleDesc = MSAA_;
    texture2dDesc.SampleDesc.Count = 1;
    texture2dDesc.SampleDesc.Quality = 0;
    texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
    texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    texture2dDesc.CPUAccessFlags = 0;
    texture2dDesc.MiscFlags = 0;
    //HRESULT hr = device_->CreateTexture2D(&texture2dDesc, NULL, depthStencilTexture_.GetAddressOf());
    HRESULT hr = device_->CreateTexture2D(&texture2dDesc, NULL, depthStencilBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    hr = device_->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView_.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
#endif // 0

    //デプスステンシルステート作成
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;


    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = FALSE;
    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<int>(DS_State::None)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<int>(DS_State::Less)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER;
    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<int>(DS_State::Greater)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<int>(DS_State::LEqual)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<int>(DS_State::GEqual)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_EQUAL;
    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<int>(DS_State::Equal)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;
    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<int>(DS_State::NotEqual)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<int>(DS_State::Always)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = FALSE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<int>(DS_State::None_No_Write)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<int>(DS_State::Less_No_Write)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER;
    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<int>(DS_State::Greater_No_Write)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<int>(DS_State::LEqual_No_Write)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<int>(DS_State::GEqual_No_Write)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_EQUAL;
    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<int>(DS_State::Equal_No_Write)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;
    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<int>(DS_State::NotEqual_No_Write)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<int>(DS_State::Always_No_Write)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    //シェーダーリソースビュー設定
    D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
    ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    srvd.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
    srvd.Texture2D.MostDetailedMip = 0;
    srvd.Texture2D.MipLevels = 1;

    //シェーダーリソースビュー生成
    hr = device->CreateShaderResourceView(depthStencilTexture.Get(), &srvd, instance->shaderResourceView_.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool DXSystem::InitializeRenderTarget()
{
    //バックバッファ取得
    ID3D11Texture2D* backBuffer = nullptr;
    HRESULT hr = instance->swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&backBuffer));

    if (FAILED(hr))
    {
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr)); 
        return false;
    }

    //レンダーターゲットビュー生成
    hr = instance->device_->CreateRenderTargetView(backBuffer, nullptr, instance->renderTargetView_.GetAddressOf());
    backBuffer->Release();
    backBuffer = nullptr;

    if (FAILED(hr))
    {
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
        return false;
    }

    //深度ステンシルバッファ生成
    CreateDepthStencil();

    //ビューポート設定
    SetViewport(screenWidth_, screenHeight_);

    return true;
}

bool DXSystem::CreateRasterizerState()
{
    D3D11_RASTERIZER_DESC rd;
    for (int state = 0; state < Rasterizer_Type; state++)
    {
        switch (static_cast<RS_State>(state))
        {
            case RS_State::Cull_Back:
                ZeroMemory(&rd, sizeof(rd));
                rd.FillMode = D3D11_FILL_SOLID;	 // 塗りつぶし
                rd.CullMode = D3D11_CULL_BACK;	 // カリング
                //rd.FrontCounterClockwise = TRUE;// 三角形の時計回りが正面
                rd.FrontCounterClockwise = FALSE; 
                rd.DepthBias = 0;
                rd.DepthBiasClamp = 0;
                rd.SlopeScaledDepthBias = 0;
                rd.DepthClipEnable = TRUE;	     // 距離に基づくクリッピングを有効か
                rd.ScissorEnable = FALSE;	     // シザー長方形カリングを有効か
                rd.MultisampleEnable = TRUE;	 // MSAAで四辺形かアルファ線を設定
                rd.AntialiasedLineEnable = TRUE; // ラインAAが有効か
                break;

            case RS_State::Cull_Front:
                ZeroMemory(&rd, sizeof(rd));
                rd.FillMode = D3D11_FILL_SOLID;
                rd.CullMode = D3D11_CULL_FRONT;
                rd.FrontCounterClockwise = TRUE;
                rd.DepthBias = 0;
                rd.DepthBiasClamp = 0;
                rd.SlopeScaledDepthBias = 0;
                rd.DepthClipEnable = TRUE;
                rd.ScissorEnable = FALSE;
                rd.MultisampleEnable = TRUE;
                rd.AntialiasedLineEnable = TRUE;
                break;

            case RS_State::Cull_None:
                ZeroMemory(&rd, sizeof(rd));
                rd.FillMode = D3D11_FILL_SOLID;
                rd.CullMode = D3D11_CULL_NONE;
                rd.FrontCounterClockwise = TRUE;
                rd.DepthBias = 0;
                rd.DepthBiasClamp = 0;
                rd.SlopeScaledDepthBias = 0;
                rd.DepthClipEnable = TRUE;
                rd.ScissorEnable = FALSE;
                rd.MultisampleEnable = FALSE;
                rd.AntialiasedLineEnable = TRUE;
                break;

            case RS_State::Standard:
                ZeroMemory(&rd, sizeof(rd));
                rd.FillMode = D3D11_FILL_SOLID;
                rd.CullMode = D3D11_CULL_NONE;
                rd.FrontCounterClockwise = TRUE;
                rd.DepthBias = 0;
                rd.DepthBiasClamp = 0;
                rd.SlopeScaledDepthBias = 0;
                rd.DepthClipEnable = TRUE;
                rd.ScissorEnable = FALSE;
                rd.MultisampleEnable = TRUE;
                rd.AntialiasedLineEnable = TRUE;
                break;

            case RS_State::Wire:
                ZeroMemory(&rd, sizeof(rd));
                rd.FillMode = D3D11_FILL_WIREFRAME;
                rd.CullMode = D3D11_CULL_BACK;
                rd.FrontCounterClockwise = TRUE;
                rd.DepthBias = 0;
                rd.DepthBiasClamp = 0;
                rd.SlopeScaledDepthBias = 0;
                rd.DepthClipEnable = TRUE;
                rd.ScissorEnable = FALSE;
                rd.MultisampleEnable = TRUE;
                rd.AntialiasedLineEnable = TRUE;
                break;
        }
        const HRESULT hr = instance->device_->CreateRasterizerState(&rd, instance->rasterizerStates_[state].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    return true;
}

bool DXSystem::CreateBlendState()
{
    D3D11_BLEND_DESC bd;

    for (int state = 0; state < Blend_Type; state++)
    {
        switch (static_cast<BS_State>(state))
        {
        case BS_State::Off:
            ZeroMemory(&bd, sizeof(bd));
            bd.IndependentBlendEnable = false;
            bd.AlphaToCoverageEnable = false;
            bd.RenderTarget[0].BlendEnable = false;
            bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

            bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
            bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            break;

        case BS_State::Alpha:
            ZeroMemory(&bd, sizeof(bd));
            bd.IndependentBlendEnable = false;
            bd.AlphaToCoverageEnable = false;
            bd.RenderTarget[0].BlendEnable = true;
            bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

            bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
            bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            break;

        case BS_State::Alpha_Test:
            ZeroMemory(&bd, sizeof(bd));
            bd.IndependentBlendEnable = false;
            bd.AlphaToCoverageEnable = true;
            bd.RenderTarget[0].BlendEnable = true;
            bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

            bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
            //bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
            bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            break;

        case BS_State::Transparent:
            ZeroMemory(&bd, sizeof(bd));
            bd.IndependentBlendEnable = false;
            bd.AlphaToCoverageEnable = false;
            bd.RenderTarget[0].BlendEnable = true;
            bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

            bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
            bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            break;

        case BS_State::Add:
            ZeroMemory(&bd, sizeof(bd));
            bd.IndependentBlendEnable = false;
            bd.AlphaToCoverageEnable = false;
            bd.RenderTarget[0].BlendEnable = true;
            bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
            bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

            bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
            bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
            bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            break;

        case BS_State::Subtract:
            ZeroMemory(&bd, sizeof(bd));
            bd.IndependentBlendEnable = false;
            bd.AlphaToCoverageEnable = false;
            bd.RenderTarget[0].BlendEnable = true;
            bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
            bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;

            bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
            bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
            bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            break;

        case BS_State::Replace:
            ZeroMemory(&bd, sizeof(bd));
            bd.IndependentBlendEnable = false;
            bd.AlphaToCoverageEnable = false;
            bd.RenderTarget[0].BlendEnable = true;
            bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
            bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

            bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
            bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
            bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            break;
        case BS_State::Multiply:
            ZeroMemory(&bd, sizeof(bd));
            bd.IndependentBlendEnable = false;
            bd.AlphaToCoverageEnable = false;
            bd.RenderTarget[0].BlendEnable = true;
            bd.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
            bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
            bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

            bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
            bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
            bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            break;
        case BS_State::GBuffer:
            ZeroMemory(&bd, sizeof(bd));
            bd.AlphaToCoverageEnable = FALSE;
            bd.IndependentBlendEnable = FALSE;
            for (int i = GB_BaseColor; i < GB_Max; i++)
            {
                bd.RenderTarget[i].BlendEnable = FALSE;
                bd.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
                bd.RenderTarget[i].DestBlend = D3D11_BLEND_ZERO;
                bd.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
                bd.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
                bd.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
                bd.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
                bd.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            }
            break;
        }
        //ブレンドステート生成
        const HRESULT hr = instance->device_->CreateBlendState(&bd, instance->blendStates_[state].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    return true;
}

void DXSystem::ReportLiveObjectsWrapper()
{
#if defined(_DEBUG)
    typedef HRESULT(__stdcall* fPtr)(const IID&, void**);
    HMODULE hDll = GetModuleHandleW(L"dxgidebug.dll");
    fPtr DXGIGetDebugInterface = (fPtr)GetProcAddress(hDll, "DXGIGetDebugInterface");
    DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&instance->dxgiDebug_);
    instance->dxgiDebug_->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
#endif
}