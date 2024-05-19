#ifndef INCLUDED_IMGUI_CTRL_H
#define INCLUDED_IMGUI_CTRL_H

#if _DEBUG
#define USE_IMGUI 1
#else
#define USE_IMGUI 0
#endif // _DEBUG


//-----< pragma >---------------------
#pragma once

//-----< pragma >---------------------

#if USE_IMGUI
#include <Windows.h>
#include <d3d11.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#endif

#if USE_IMGUI
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern const ImWchar glyphRangesJapanese[]; // ref :imgui_ja_gryph_ranges.cpp
#endif // USE_IMGUI

//============================================
// 
//      ImGuiCtrl
// 
//============================================
#if USE_IMGUI
namespace ImGuiCtrl
{
    void Initialize(HWND windowHandle, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

    // ImGui::Begin()ëOÇ…åƒÇ—èoÇ∑
    void ClearFrame();

    // ï`âÊåãâ ÇèoóÕ
    void Display();

    void Uninitialize();

    void SetImguiStyle();
}

#endif // USE_IMGUI

//-----< macro >--------------------------------
#if USE_IMGUI
#define IMGUI_CTRL_WND_PRC_HANDLER(hwnd, msg, wParam, lParam)\
if(ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))return true
#define IMGUI_CTRL_INITIALIZE(hwnd, device, context) ImGuiCtrl::Initialize(hwnd, device, context)
#define IMGUI_CTRL_CLEAR_FRAME() ImGuiCtrl::ClearFrame()
#define IMGUI_CTRL_DISPLAY() ImGuiCtrl::Display()
#define IMGUI_CTRL_UNINITIALIZE() ImGuiCtrl::Uninitialize();

#else
#define IMGUI_CTRL_WND_PRC_HANDLER(hwnd, msg, wParam, lParam)
#define IMGUI_CTRL_INITIALIZE(hwnd, device, context)
#define IMGUI_CTRL_CLEAR_FRAME()
#define IMGUI_CTRL_DISPLAY()
#define IMGUI_CTRL_UNINITIALIZE()

#endif // USE_IMGUI

#endif // INCLUDED_IMGUI_CTRL_H