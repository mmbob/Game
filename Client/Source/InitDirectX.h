#pragma once

#include <d3d9.h>
#include <d3dx9.h>

class DirectXManager
{
	LPDIRECT3D9 pDirectX;
	LPDIRECT3DDEVICE9 pDevice;
	LPD3DXSPRITE pSprite;
	LPD3DXFONT pFont;
public:
	void Init();
	void InitDeviceObjects(HWND window, int width, int height);
	void ReleaseDeviceObjects();
};