#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>

using namespace std;

#include "Game.h"
#include "InitDirectX.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

int WINAPI wWinMain(HINSTANCE, HINSTANCE instance, LPWSTR, int)
{
	g_pGameClient = new GameClient();

	g_pGameClient->Init(instance);

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



		lastFrame = currentFrame;
	}

	delete g_pGameClient;

	return msg.wParam;
}