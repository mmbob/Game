#pragma once

#include <Windows.h>

#include "GameState.h"
#include "DirectX.h"

class GameClient : public GameState
{
	static const wchar_t* WindowClassName;
	static const wchar_t* WindowTitle;

	static const int ResolutionX = 800;
	static const int ResolutionY = 600;

	bool needToQuit;
	HINSTANCE instance;
	HWND mainWindow;
	DirectXManager directX;
	std::unique_ptr<Renderer> renderer;
	int startDay;
	int fps;

	bool InitInstance(HINSTANCE instance);
	void InitFonts();
	void CreateMainWindow(int width, int height, bool fullScreen);

	static LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
public:
	GameClient();

	void Init(HINSTANCE instance);
	void UnInit();

	virtual void Update(float timeElapsed);
	virtual void Render(float timeElapsed);
	virtual void Input(float timeElapsed);

	void SetStartDay(int startDay);
	int GetStartDay() const;

	int GetFPS() const;

	int MainLoop();

	void Quit();
};

extern GameClient* g_pGameClient;