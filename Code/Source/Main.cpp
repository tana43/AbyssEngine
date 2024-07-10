#include <Windows.h>
#include <memory>
#include <Windows.Foundation.h>
#include <wrl\wrappers\corewrappers.h>
#include <wrl\client.h>
#include <stdio.h>
#include <sstream>

#include <../External/DirectXTK-main/Inc/Keyboard.h>
#include <../External/DirectXTK-main/Inc/Mouse.h>

#include "Misc.h"
#include "Engine.h"
#include "imgui/ImGuiCtrl.h"

using namespace ABI::Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	IMGUI_CTRL_WND_PRC_HANDLER(hWnd, message, wParam, lParam);

	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:

	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}

		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;
	case WM_SETFOCUS:
		
		break;
	case WM_KILLFOCUS:
		
		break;
	case WM_ACTIVATE:
	case WM_ACTIVATEAPP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;

	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
		{
			// This is where you'd implement the classic ALT+ENTER hotkey for fullscreen toggle
		}
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return(DefWindowProc(hWnd, message, wParam, lParam));
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    //ウィンドウ生成
    TCHAR szWindowClass[] = TEXT("Abyss Engine");
    WNDCLASS wcex;
    wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = nullptr;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szWindowClass;
	RegisterClass(&wcex);

	const HWND hwnd = CreateWindow(
		szWindowClass,
		szWindowClass,
		WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, 
		AbyssEngine::DXSystem::GetScreenWidth(),
		AbyssEngine::DXSystem::GetScreenHeight(),
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	ShowWindow(hwnd, nCmdShow);

	RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);

	//DXSystem生成
	std::unique_ptr<AbyssEngine::DXSystem> DxSystem = std::make_unique<AbyssEngine::DXSystem>();


	//デバイス初期化
	if (!AbyssEngine::DXSystem::Initialize(hwnd))
	{
		return 0;
	}

	//エンジン作成
	auto engine = std::make_unique<AbyssEngine::Engine>();
	AbyssEngine::Time::timeScale_ = 1.0f;

	//メインループ
	MSG hmsg = { nullptr };
	float interval = 1.0f;
	ULONGLONG before = GetTickCount64();
	int fps = 0;

	while (hmsg.message != WM_QUIT)
	{
		if (PeekMessage(&hmsg,nullptr,0,0,PM_REMOVE))
		{
			TranslateMessage(&hmsg);
			DispatchMessage(&hmsg);
		}
		else
		{
			//Timeクラス更新
			AbyssEngine::Time::deltaTime_ = static_cast<float>(GetTickCount64() - before) * 0.001f * AbyssEngine::Time::timeScale_;

			//FPSを算出し表示
			before = GetTickCount64();
			const float mspf = 1000.0f / static_cast<float>(fps);

			interval -= AbyssEngine::Time::deltaTime_;
			++fps;
			if (interval < 0)
			{
				std::ostringstream outs;
				outs.precision(6);
				outs << "Abyss Engine     " << "FPS : " << fps << " / " << "Frame Time : " << mspf << "(ms)";
				SetWindowTextA(hwnd, outs.str().c_str());
				interval += 1.0f;
				fps = 0;
			}

			//更新・描画
			engine->Update();
		}
	}

	engine.reset();

	//delete DxSystem;

	return 0;
}