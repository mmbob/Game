#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

enum MouseButton
{
	MB_Left,
	MB_Right,
	MB_Middle,
	MB_4,
	MB_5,
	MB_6,
	MB_7,
	MB_8,
};

class DirectXManager
{
	// Rendering
	LPDIRECT3D9 pDirectX;
	LPDIRECT3DDEVICE9 pDevice;
	LPD3DXSPRITE pSprite;

	// Input
	LPDIRECTINPUT8 pInput;
	LPDIRECTINPUTDEVICE8 pMouse;
	LPDIRECTINPUTDEVICE8 pKeyboard;
	DIMOUSESTATE mouseState;
	unsigned char keyState[256];
public:
	void Init();
	void UnInit();
	void InitDeviceObjects(HWND window, int width, int height);
	void ReleaseDeviceObjects();

	LPDIRECT3DDEVICE9 GetDevice() const;
	LPD3DXSPRITE GetSprite() const;

	void Update();

	bool IsKeyPressed(int keyCode) const;
	int GetMouseX() const;
	int GetMouseY() const;
	int GetMouseWheel() const;
	bool IsMousePressed(MouseButton button) const;
};