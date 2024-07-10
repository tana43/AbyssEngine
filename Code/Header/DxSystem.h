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
		//�K�v�Ȃ�ǉ��i�V�F�[�_�[���̑Ή����K�{�j
		GB_Max,
	};

	//DirectX���b�p�[�N���X
	//���������[�N�N�����̂�static��߂đf���ɍ��܂�...
	class DXSystem
	{
	private:
		static DXSystem* instance;
	public:
		DXSystem();
		~DXSystem();
		
		static HWND hwnd_;

		//MSAA�g�p���̃p�����[�^
		static DXGI_SAMPLE_DESC MSAA_; 

		static bool Initialize(HWND hWnd);//������
		static void Release();//��n��
		static void Clear();//�����_�[�^�[�Q�b�g�̃N���A
		static void GBufferClearAndSetTarget();//GBuffer�̃N���A�Əo�͐�̕ύX
		static void Flip(int n = 0);//�t���b�v����

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
		static void SetViewport(int width, int height_, int num = 1);//�r���[�|�[�g���Z�b�g
		static void SetDefaultView();//���C���E�B���h�E�̃����_�[�^�[�Q�b�g���Z�b�g����

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

		static HRESULT CreateDevice();         //�f�o�C�X�̍쐬
		static bool CreateDepthStencil();     //�f�v�X�X�e���V���e�N�X�`���̍쐬
		static bool InitializeRenderTarget(); //�����_�[�^�[�Q�b�g�̏�����

		static constexpr int Rasterizer_Type = 5; //�p�ӂ��ꂽ���X�^���C�U�[�X�e�[�g�̐�
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates_[Rasterizer_Type];
		static bool CreateRasterizerState(); //���X�^���C�U�[�X�e�[�g�̍쐬

		static constexpr int Blend_Type = 9; //�p�ӂ��ꂽ�u�����h�X�e�[�g�̐�
		Microsoft::WRL::ComPtr<ID3D11BlendState>	blendStates_[Blend_Type];
		static bool CreateBlendState(); //�u�����h�X�e�[�g�̍쐬

		// DirectX11�̏�������A���������[�N���o�̂��߂Ɏ��̊֐����Ăяo��
		static void ReportLiveObjectsWrapper();

	};

	

}