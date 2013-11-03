#include "Game.h"

#include <windowsx.h>
#include <cassert>

#include "SanityCrystal.h"

const wchar_t* GameClient::WindowClassName = L"fa3a766d-cc01-4644-98fe-fa9008e7a20d";
const wchar_t* GameClient::WindowTitle = L"Nightmares";

GameClient* g_pGameClient = nullptr;

GameClient::GameClient() : renderer(), player(&renderer)
{
	srand(GetTickCount());
}

bool GameClient::InitInstance(HINSTANCE instance)
{
	HANDLE mutex = CreateMutex(nullptr, false, L"31c4b2f9-8bde-490a-91bc-fca8f4a89c42");
	if (mutex == nullptr)
		return false;

	if (GetLastError() == ERROR_ALREADY_EXISTS)		// Another game window is already open
	{
		HWND otherWindow = FindWindow(WindowClassName, nullptr);

		SetForegroundWindow(otherWindow);

		return false;
	}

	this->instance = instance;

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.hInstance = instance;
	wc.lpszClassName = WindowClassName;
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WinProc;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = GetStockBrush(BLACK_BRUSH);

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

	mainWindow = CreateWindowEx(exStyles, WindowClassName, WindowTitle, styles, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, HWND_DESKTOP, nullptr, instance, nullptr);

	GetClientRect(mainWindow, &rc);
	assert(rc.right == width && rc.bottom == height);
}

LRESULT CALLBACK GameClient::WinProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:

		break;

	case WM_ERASEBKGND:
		break;

	case WM_CLOSE:
		DestroyWindow(window);
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

	directX.Init();
	directX.InitDeviceObjects(mainWindow, 800, 600);

	player.SetPosition(D3DXVECTOR3(400.0f, 400.0f, 0.5f));

	engine.Init();
	engine.AddEntity(&player, EntityType::Dynamic);

	SanityCrystal** crystals = new SanityCrystal*[3];
	for (int i = 0; i < 3; ++i)
	{
		crystals[i] = new SanityCrystal(&renderer);
		crystals[i]->SetPosition(D3DXVECTOR3(float(rand() % 750), float(rand() % 450), 1.0f));

		engine.AddEntity(crystals[i], EntityType::Static);
	}

	renderer.Init(&directX, mainWindow);
}

void GameClient::Input()
{
	directX.Update();
}

void GameClient::Update()
{
	player.Update(&directX);
	engine.Update();

	
}

void GameClient::Render()
{
	renderer.Render();
}

int GameClient::MainLoop()
{
	MSG msg;

	bool quit = false;
	const int timePerFrame = 1000 / 60;
	ULONGLONG lastFrame = GetTickCount64();
	while (!quit)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;

		ULONGLONG currentFrame = GetTickCount64();
		if (currentFrame - lastFrame < timePerFrame)
			Sleep(timePerFrame - (currentFrame - lastFrame));

		Input();
		Update();
		Render();

		lastFrame = currentFrame;
	}

	return msg.wParam;
}