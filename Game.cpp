#include "Game.h"

#include <cassert>

const wchar_t* GameClient::WindowClassName = L"fa3a766d-cc01-4644-98fe-fa9008e7a20d";
const wchar_t* GameClient::WindowTitle = L"Nightmares";

GameClient* g_pGameClient = nullptr;

bool GameClient::InitInstance(HINSTANCE instance)
{
	HANDLE mutex = CreateMutex(nullptr, false, L"31c4b2f9-8bde-490a-91bc-fca8f4a89c42");
	if (mutex == nullptr)
		return false;

	if (GetLastError() == ERROR_ALREADY_EXISTS)		// Another game window is already open
	{
		HWND otherWindow = FindWindow(WindowClassName, nullptr);
	}

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.lpszClassName = WindowClassName;
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WinProc;

	bool success = (RegisterClassEx(&wc) != 0);
	if (!success)
		return false;

	return true;
}

void GameClient::CreateMainWindow(int width, int height, bool fullScreen)
{
	DWORD exStyles = WS_EX_OVERLAPPEDWINDOW;
	DWORD styles = WS_VISIBLE | WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME;

	RECT rc;
	rc.left = 0; rc.top = 0; rc.right = width; rc.bottom = height;	// fix

	AdjustWindowRectEx(&rc, styles, false, exStyles);	// check

	HWND window = CreateWindowEx(exStyles, WindowClassName, WindowTitle, styles, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, HWND_DESKTOP, nullptr, instance, nullptr);

	GetClientRect(window, &rc);
	assert(rc.right == width && rc.bottom == height);
}

LRESULT CALLBACK GameClient::WinProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:

		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(window, message, wParam, lParam);
	}

	return 0;
}

void GameClient::Init(HINSTANCE instance)
{
	InitInstance(instance);

	CreateMainWindow(ResolutionX, ResolutionY, false);
}