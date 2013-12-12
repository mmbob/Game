#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>

using namespace std;

#include "Game.h"
#include "Engine.h"
#include "Renderer.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#pragma comment(lib, "box2dd.lib")
#pragma comment(lib, "libjsond.lib")
#else
#pragma comment(lib, "box2d.lib")
#pragma comment(lib, "libjson.lib")
#endif


int WINAPI wWinMain(HINSTANCE, HINSTANCE instance, LPWSTR, int)
{
	g_pGameClient = new GameClient;

	g_pGameClient->Init(instance);
	int returnCode = g_pGameClient->MainLoop();

	return returnCode;
}