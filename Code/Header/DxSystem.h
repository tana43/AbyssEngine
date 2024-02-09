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
		wchar_t buffer[256]; \
		wsprintfW(buffer, str, __VA_ARGS__); \
		OutputDebugString(buffer); \
      }

namespace AbyssEngine
{
	//DepthStencilState
	enum class DS_State { None, Less, Greater, LEqual, GEqual, Equal, NotEqual, Always, None_No_Write, Less_No_Write, Greater_No_Write, LEqual_No_Write, GEqual_No_Write, Equal_No_Write, NotEqual_No_Write, Always_No_Write };
	//RastarizerState
	enum class RS_State {Cull_Back,Cull_Front,Cull_None,Standard,Wire};
	//BlendState
	enum class BS_State { Off, Alpha, Alpha_Test, Transparent, Add, Subtract, Replace, Multiply };

	//DirectX���b�p�[�N���X
	class DXSystem
	{
	public:
		static Microsoft::WRL::ComPtr<ID3D11Device> device_;
		static Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext_;
		static HWND hwnd_;

		//MSAA�g�p���̃p�����[�^
		static DXGI_SAMPLE_DESC MSAA_; 

		static bool Initialize(HWND hWnd, int width, int height);//������
		static void Release();//��n��
		static void Clear();//�����_�[�^�[�Q�b�g�̃N���A
		static void Flip(int n = 0);//�t���b�v����

		static int GetScreenWidth() { return screenWidth_; }
		static int GetScreenHeight() { return screenHeight_; }
		static ID3D11DepthStencilView* GetDepthStencilView() { return depthStencilView_.Get(); }
		static ID3D11RenderTargetView* GetRenderTargetView() { return renderTargetView_.Get(); }
		static ID3D11ShaderResourceView* GetShaderResourceView() { return shaderResourceView_.Get(); }
		static ID3D11DepthStencilState* GetDepthStencilState(DS_State state) { return depthStencilStates_[static_cast<int>(state)].Get(); }
		static ID3D11RasterizerState* GetRasterizerState(RS_State state) { return rasterizerStates_[static_cast<int>(state)].Get(); }
		static ID3D11BlendState* GetBlendState(BS_State state) { return blendStates_[static_cast<int>(state)].Get(); }
		static void SetViewport(int width, int height, int num = 1);//�r���[�|�[�g���Z�b�g
		static void SetDefaultView();//���C���E�B���h�E�̃����_�[�^�[�Q�b�g���Z�b�g����


	private:
		static int screenWidth_;
		static int screenHeight_;

		static Microsoft::WRL::ComPtr<IDXGISwapChain>			swapChain_;
		static Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView_;
		static Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilTexture_;
		static Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView_;
		static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView_;
		static Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilStates_[16];

		static Microsoft::WRL::ComPtr<IDXGIDebug>               dxgiDebug_;

		static HRESULT CreateDevice();         //�f�o�C�X�̍쐬
		static bool CreateDepthStencil();     //�f�v�X�X�e���V���e�N�X�`���̍쐬
		static bool InitializeRenderTarget(); //�����_�[�^�[�Q�b�g�̏�����

		static constexpr int rasterizerType_ = 5; //�p�ӂ��ꂽ���X�^���C�U�[�X�e�[�g�̐�
		static Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates_[rasterizerType_];
		static bool CreateRasterizerState(); //���X�^���C�U�[�X�e�[�g�̍쐬

		static constexpr int blendType_ = 8; //�p�ӂ��ꂽ�u�����h�X�e�[�g�̐�
		static Microsoft::WRL::ComPtr<ID3D11BlendState>	blendStates_[blendType_];
		static bool CreateBlendState(); //�u�����h�X�e�[�g�̍쐬

	};

	

}