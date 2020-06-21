#pragma once

#include "Window.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Interface/imgui/imgui.h"
#include "Interface/imgui/imgui_impl_win32.h"
#include <memory>
#include "Input/InputManager.h"
#include <Windowsx.h>

LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class DX11Window : public Window {
private:
	HINSTANCE hInst;
	HWND hWnd;
public:
	DX11Window(int width, int height) :
		Window(width, height) 
	{
		hInst = GetModuleHandle(NULL);

		// Creating window class
		WNDCLASSEX windowClass = {};
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_CLASSDC;
		windowClass.lpfnWndProc = _HandleMsgSetup;
		windowClass.cbClsExtra = 0;
		windowClass.hInstance = hInst;
		windowClass.hIcon = nullptr;
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.hbrBackground = nullptr;
		windowClass.lpszMenuName = nullptr;
		windowClass.lpszClassName = "HollowAppClass";
		RegisterClassEx(&windowClass);

		// Center positions
		int centerScreenX = GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
		int centerScreenY = GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;

		// Creating rectangle
		RECT windowRect;
		windowRect.left = centerScreenX;
		windowRect.right = width + windowRect.left;
		windowRect.top = centerScreenY;
		windowRect.bottom = height + windowRect.top;
		AdjustWindowRect(&windowRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

		int styles = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME;

		hWnd = CreateWindow("HollowAppClass", "Hollow", styles,
			windowRect.left, windowRect.top,
			windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
			nullptr, nullptr, hInst, this);

		RAWINPUTDEVICE Rid[2];

		Rid[0].usUsagePage = 0x01;
		Rid[0].usUsage = 0x02;
		Rid[0].dwFlags = 0;   // adds HID mouse and also ignores legacy mouse messages
		Rid[0].hwndTarget = 0;

		RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
		//ShowCursor(false);
		ShowWindow(hWnd, SW_SHOW); // SW_SHOWMAXIMIZED
		UpdateWindow(hWnd);
	}

	static LRESULT __stdcall _HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// retrieve ptr to window class
		DX11Window* const pWnd = reinterpret_cast<DX11Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		// forward message to window class handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	static LRESULT __stdcall _HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
		if (msg == WM_NCCREATE)
		{
			// extract ptr to window class from creation data
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			DX11Window* const pWnd = reinterpret_cast<DX11Window*>(pCreate->lpCreateParams);
			// set WinAPI-managed user data to store ptr to window class
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
			// set message proc to normal (non-setup) handler now that setup is finished
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&DX11Window::_HandleMsgThunk));
			// forward message to window class handler
			return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
		}
		// if we get a message before the WM_NCCREATE message, handle with default handler
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
			return true;
		}

		LRESULT result = 0;
		switch (msg) {
		case WM_INPUT: {
			UINT dataSize = 0;
			GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));
			if (dataSize > 0) {
				std::unique_ptr<BYTE[]> rawData = std::make_unique<BYTE[]>(dataSize);
				if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawData.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize) {
					RAWINPUT* input = reinterpret_cast<RAWINPUT*>(rawData.get());
					if (input->header.dwType == RIM_TYPEMOUSE) {
						InputManager::instance()->mouseMoveX = input->data.mouse.lLastX;
						InputManager::instance()->mouseMoveY = input->data.mouse.lLastY;
					}
				}
			}
		} break;
		case WM_CLOSE: {
			isClosed = true;
		} break;
		case WM_KEYDOWN: {
			InputManager::instance()->setKeyPressed(wParam, true);
		} break;
		case WM_KEYUP: {
			InputManager::instance()->setKeyPressed(wParam, false);
		} break;
		// Mouse events handling
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN: {
			InputManager::instance()->setKeyPressed(wParam, true);
		} break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP: {
			InputManager::instance()->setKeyPressed(wParam, false);
		} break;
		case WM_MOUSEMOVE: {
			InputManager::instance()->mousePosX = GET_X_LPARAM(lParam);
			InputManager::instance()->mousePosY = GET_Y_LPARAM(lParam);
		} break;
		case WM_SIZE: {
			/*if (wParam == SIZE_RESTORED) {
				newWindowWidth = LOWORD(lParam);
				newWindowHeight = HIWORD(lParam);

				if (newWindowWidth != width || newWindowHeight != height) {
					hasResizeEvent = true;
				}
			}*/
		} break;
		default:
			result = DefWindowProc(hWnd, msg, wParam, lParam);
			break;
		}
		return result;
	}

	virtual bool isOpen() override {
		return !isClosed;
	}

	virtual void pollEvents() override {
		MSG message;
		while (PeekMessage(&message, hWnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	HWND getHwnd() { return hWnd; }
	HINSTANCE getHInstances() { return hInst; }
};