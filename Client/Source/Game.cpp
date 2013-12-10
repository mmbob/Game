#include "Game.h"

#include <windowsx.h>
#include <cassert>

#include "PhysicsDebug.h"
#include "SanityCrystal.h"
#include "Enemy.h"
#include "Util.h"
#include "WorldGenerator.h"

const wchar_t* GameClient::WindowClassName = L"fa3a766d-cc01-4644-98fe-fa9008e7a20d";
const wchar_t* GameClient::WindowTitle = L"Nightmares";

GameClient* g_pGameClient = nullptr;

GameClient::GameClient()
: renderer(new Renderer), GameState(nullptr, renderer.get())
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

void GameClient::InitFonts()
{
	HDC context = GetDC(mainWindow);
	const int LargeHeight = -MulDiv(32, GetDeviceCaps(context, LOGPIXELSY), 72);
	const int MediumHeight = -MulDiv(24, GetDeviceCaps(context, LOGPIXELSY), 72);
	const int SmallHeight = -MulDiv(16, GetDeviceCaps(context, LOGPIXELSY), 72);
	ReleaseDC(mainWindow, context);

	D3DXFONT_DESC desc;
	desc.Height = LargeHeight;
	desc.Width = 0;
	desc.OutputPrecision = OUT_DEFAULT_PRECIS;
	desc.CharSet = DEFAULT_CHARSET;
	desc.Italic = false;
	desc.Weight = FW_ULTRALIGHT;
	desc.PitchAndFamily = DEFAULT_PITCH || FF_DONTCARE;
	desc.Quality = DEFAULT_QUALITY;
	wcscpy_s(desc.FaceName, L"Tahoma");

	LPD3DXFONT uiSmallFont;
	LPD3DXFONT uiMediumFont;
	LPD3DXFONT uiLargeFont;

	HRESULT result = D3DXCreateFontIndirect(directX.GetDevice(), &desc, &uiLargeFont);
	assert(SUCCEEDED(result));

	desc.Height = MediumHeight;

	result = D3DXCreateFontIndirect(directX.GetDevice(), &desc, &uiMediumFont);
	assert(SUCCEEDED(result));

	desc.Height = SmallHeight;

	result = D3DXCreateFontIndirect(directX.GetDevice(), &desc, &uiSmallFont);
	assert(SUCCEEDED(result));

	uiLargeFont->PreloadCharacters('0', '9');
	uiLargeFont->PreloadCharacters('-', '-');
	uiLargeFont->PreloadCharacters('+', '+');

	uiMediumFont->PreloadCharacters('0', '9');
	uiMediumFont->PreloadCharacters('a', 'a');
	uiMediumFont->PreloadCharacters('p', 'p');
	uiMediumFont->PreloadCharacters('m', 'm');
	uiMediumFont->PreloadCharacters(':', ':');

	renderer->AddFont(L"UILarge", uiLargeFont);
	renderer->AddFont(L"UIMedium", uiMediumFont);
	renderer->AddFont(L"UISmall", uiSmallFont);
	uiLargeFont->Release();
	uiMediumFont->Release();
	uiSmallFont->Release();
}

void GameClient::UnInit()
{ }

void GameClient::CreateMainWindow(int width, int height, bool)
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

	case WM_ACTIVATE:
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

	renderer->Init(&directX, mainWindow);
	renderer->SetGameArea(Rect(0, 0, ResolutionX, ResolutionY - 100));

	InitFonts();

	EnterState(new InGameState(this, renderer.get()));
}

void GameClient::Input(float timeElapsed)
{
	directX.Update();

	if (child != nullptr)
		child->Input(timeElapsed);
}

void GameClient::Update(float timeElapsed)
{
	if (child != nullptr)
		child->Update(timeElapsed);
}

void GameClient::Render(float timeElapsed)
{
	renderer->Render();

	if (child != nullptr)
		child->Render(timeElapsed);
}

int GameClient::MainLoop()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	bool quit = false;
	const int TimePerFrame = 1000 / 60;
	LARGE_INTEGER lastFrame;
	QueryPerformanceCounter(&lastFrame);
	while (!quit)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;

		LARGE_INTEGER currentFrame;
		QueryPerformanceCounter(&currentFrame);

		float timeElapsed = float(currentFrame.QuadPart - lastFrame.QuadPart) / float(frequency.QuadPart);

		if (int(timeElapsed * 1000) < TimePerFrame)
			Sleep(TimePerFrame - int(timeElapsed * 1000));

		LARGE_INTEGER oldLastFrame = lastFrame;
		lastFrame = currentFrame;
		QueryPerformanceCounter(&currentFrame);

		timeElapsed = float(currentFrame.QuadPart - oldLastFrame.QuadPart) / float(frequency.QuadPart);

		Input(timeElapsed);
		Update(timeElapsed);
		Render(timeElapsed);
	}

	return msg.wParam;
}