#pragma once
#include <dxgidebug.h>
#include <d3d11.h>
#include <wrl.h>
#include <Windows.h>
#define SAFE_RELEASE(x) if((x)){(x)->Release();(x)=NULL;}
#define SAFE_DELETE(x) if((x)){delete (x);(x)=NULL;}
#define SAFE_DELETE_ARRAY(x) if(x){delete[] x; x=0;}

#define Debug_Log( str, ... ) \
      { \
		wchar_t buffer_[256]; \
		wsprintfW(buffer_, str, __VA_ARGS__); \
		OutputDebugString(buffer_); \
      }

namespace AbyssEngine
{
	//DepthStencilState
	enum class DS_State { None, Less, Greater, LEqual, GEqual, Equal, NotEqual, Always, None_No_Write, Less_No_Write, Greater_No_Write, LEqual_No_Write, GEqual_No_Write, Equal_No_Write, NotEqual_No_Write, Always_No_Write };
	//RastarizerState
	enum class RS_State {Cull_Back,Cull_Front,Cull_None,Standard,Wire};
	//BlendState
	enum class BS_State { Off, Alpha, Alpha_Test, Transparent, Add, Subtract, Replace, Multiply ,GBuffer};

	//G-Buffer
	enum GBufferId
	{
		GB_BaseColor = 0,
		GB_Emissive,
		GB_Normal,
		GB_Parameters,
		GB_Depth,
		//必要なら追加（シェーダー側の対応も必須）
		GB_Max,
	};

	//DirectXラッパークラス
	//メモリリーク起こすのでstaticやめて素直に作ります...
	class DXSystem
	{
	private:
		static DXSystem* instance;
	public:
		DXSystem();
		~DXSystem();
		
		static HWND hwnd_;

		//MSAA使用時のパラメータ
		static DXGI_SAMPLE_DESC MSAA_; 

		static bool Initialize(HWND hWnd);//初期化
		static void Release();//後始末
		static void Clear();//レンダーターゲットのクリア
		static void GBufferClearAndSetTarget();//GBufferのクリアと出力先の変更
		static void Flip(int n = 0);//フリップ処理

		static ID3D11Device* GetDevice() { return instance->device_.Get(); }
		static ID3D11DeviceContext* GetDeviceContext() { return instance->deviceContext_.Get(); }

		static int GetScreenWidth() { return screenWidth_; }
		static int GetScreenHeight() { return screenHeight_; }
		static ID3D11DepthStencilView* GetDepthStencilView() { return instance->depthStencilView_.Get(); }
		static ID3D11RenderTargetView* GetRenderTargetView() { return instance->renderTargetView_.Get(); }
		static ID3D11ShaderResourceView* GetShaderResourceView() { return instance->shaderResourceView_.Get(); }
		static ID3D11DepthStencilState* GetDepthStencilState(DS_State state) { return instance->depthStencilStates_[static_cast<int>(state)].Get(); }
		static ID3D11RasterizerState* GetRasterizerState(RS_State state) { return instance->rasterizerStates_[static_cast<int>(state)].Get(); }
		static ID3D11BlendState* GetBlendState(BS_State state) { return instance->blendStates_[static_cast<int>(state)].Get(); }
		static void SetViewport(int width, int height_, int num = 1);//ビューポートをセット
		static void SetDefaultView();//メインウィンドウのレンダーターゲットをセットする

		static void SetDepthStencilState(DS_State type, UINT stencilRef = 1);
		static void SetRasterizerState(RS_State type);
		static void SetBlendState(BS_State type);
	private:

		Microsoft::WRL::ComPtr<ID3D11Device> device_;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext_;

		static int screenWidth_;
		static int screenHeight_;

		Microsoft::WRL::ComPtr<IDXGISwapChain>			swapChain_;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView_;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilTexture_;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView_;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView_;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilStates_[16];

		Microsoft::WRL::ComPtr<IDXGIDebug>               dxgiDebug_;

		static HRESULT CreateDevice();         //デバイスの作成
		static bool CreateDepthStencil();     //デプスステンシルテクスチャの作成
		static bool InitializeRenderTarget(); //レンダーターゲットの初期化

		static constexpr int Rasterizer_Type = 5; //用意されたラスタライザーステートの数
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates_[Rasterizer_Type];
		static bool CreateRasterizerState(); //ラスタライザーステートの作成

		static constexpr int Blend_Type = 9; //用意されたブレンドステートの数
		Microsoft::WRL::ComPtr<ID3D11BlendState>	blendStates_[Blend_Type];
		static bool CreateBlendState(); //ブレンドステートの作成

		// DirectX11の初期化後、メモリリーク検出のために次の関数を呼び出す
		static void ReportLiveObjectsWrapper();

	};

	

}