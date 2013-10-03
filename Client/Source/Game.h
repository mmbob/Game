#pragma once

#include <Windows.h>

#include "Engine.h"
#include "Renderer.h"


class GameClient
{
	static const wchar_t* WindowClassName;
	static const wchar_t* WindowTitle;

	static const int ResolutionX = 800;
	static const int ResolutionY = 600;

	HINSTANCE instance;
	HWND mainWindow;
	Engine engine;
	DirectXManager directX;

	bool InitInstance(HINSTANCE instance);
	void CreateMainWindow(int width, int height, bool fullScreen);
	static LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
public:
	void Init(HINSTANCE instance);

	void Update();

	Renderer renderer;
};

extern GameClient* g_pGameClient;