#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

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
public:
	void Init();
	void UnInit();
	void InitDeviceObjects(HWND window, int width, int height);
	void ReleaseDeviceObjects();

	LPDIRECT3DDEVICE9 GetDevice() const;
	LPD3DXSPRITE GetSprite() const;
};