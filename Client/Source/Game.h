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
	Renderer renderer;
	Player player;
	GameWorld world;

	std::unordered_map<std::wstring, LPD3DXFONT> fonts;
	std::unordered_map<std::wstring, IRenderObject*> uiElements;

	float gameTime;

	bool InitInstance(HINSTANCE instance);
	void InitFonts();
	void CreateMainWindow(int width, int height, bool fullScreen);
	void Update(float timeElapsed);
	void Render(float timeElapsed);
	void Input(float timeElapsed);

	static LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
public:
	GameClient();

	void Init(HINSTANCE instance);
	void UnInit();

	float GetGameTime() const;

	int MainLoop();
};

extern GameClient* g_pGameClient;